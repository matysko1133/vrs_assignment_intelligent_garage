/* disp.c */
#include "disp.h"
#include "main.h"

#define LCD_MAP  0

#if (LCD_MAP == 0)

#define LCD_RS (1U << 0)
#define LCD_RW (1U << 1)
#define LCD_EN (1U << 2)
#define LCD_BL (1U << 3)

#elif (LCD_MAP == 1)

#define LCD_RS (1U << 0)
#define LCD_EN (1U << 1)
#define LCD_BL (1U << 2)
#define LCD_RW (1U << 3)

#else
#error "Unsupported LCD_MAP"
#endif

static I2C_HandleTypeDef *s_hi2c = NULL;
static uint8_t s_addr7 = 0;
static uint8_t s_addr8 = 0;
static led_state_t s_last_state = (led_state_t)255;

/* expander_write */
static HAL_StatusTypeDef expander_write(uint8_t data)
{
  return HAL_I2C_Master_Transmit(s_hi2c, s_addr8, &data, 1, 50);
}

/* pulse_enable */
static HAL_StatusTypeDef pulse_enable(uint8_t data)
{
  HAL_StatusTypeDef st;

  st = expander_write(data | LCD_EN);
  if (st != HAL_OK) return st;
  HAL_Delay(1);

  st = expander_write(data & ~LCD_EN);
  if (st != HAL_OK) return st;
  HAL_Delay(1);

  return HAL_OK;
}

/* write4bits */
static HAL_StatusTypeDef write4bits(uint8_t nibble_high, uint8_t rs)
{

  uint8_t data = (nibble_high & 0xF0) | LCD_BL;
  if (rs) data |= LCD_RS;

  HAL_StatusTypeDef st = expander_write(data);
  if (st != HAL_OK) return st;

  return pulse_enable(data);
}

/* send_byte */
static HAL_StatusTypeDef send_byte(uint8_t value, uint8_t rs)
{
  HAL_StatusTypeDef st;
  st = write4bits(value & 0xF0, rs);
  if (st != HAL_OK) return st;
  st = write4bits((value << 4) & 0xF0, rs);
  return st;
}

/* lcd_cmd */
static HAL_StatusTypeDef lcd_cmd(uint8_t cmd)
{
  HAL_StatusTypeDef st = send_byte(cmd, 0);
  if (cmd == 0x01 || cmd == 0x02) HAL_Delay(2);
  return st;
}

/* lcd_data */
static HAL_StatusTypeDef lcd_data(uint8_t data)
{
  return send_byte(data, 1);
}

/* lcd_clear */
static HAL_StatusTypeDef lcd_clear(void)
{
  return lcd_cmd(0x01);
}

/* lcd_set_cursor */
static HAL_StatusTypeDef lcd_set_cursor(uint8_t col, uint8_t row)
{
  static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row >= LCD_ROWS) row = 0;
  return lcd_cmd(0x80 | (col + row_offsets[row]));
}

/* lcd_print_padded */
static HAL_StatusTypeDef lcd_print_padded(const char *s)
{
  HAL_StatusTypeDef st;
  uint8_t i = 0;

  for (; s[i] != '\0' && i < LCD_COLS; i++)
  {
    st = lcd_data((uint8_t)s[i]);
    if (st != HAL_OK) return st;
  }
  for (; i < LCD_COLS; i++)
  {
    st = lcd_data((uint8_t)' ');
    if (st != HAL_OK) return st;
  }
  return HAL_OK;
}

/* autodetect_addr7 */
static uint8_t autodetect_addr7(void)
{

  const uint8_t candidates[] = {0x27, 0x3F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (uint32_t i = 0; i < sizeof(candidates); i++)
  {
    uint8_t a7 = candidates[i];
    if (HAL_I2C_IsDeviceReady(s_hi2c, (uint16_t)(a7 << 1), 2, 50) == HAL_OK)
      return a7;
  }
  return 0;
}

/* lcd_init_sequence */
static HAL_StatusTypeDef lcd_init_sequence(void)
{

  HAL_Delay(50);

  if (write4bits(0x30, 0) != HAL_OK) return HAL_ERROR;
  HAL_Delay(5);
  if (write4bits(0x30, 0) != HAL_OK) return HAL_ERROR;
  HAL_Delay(5);
  if (write4bits(0x30, 0) != HAL_OK) return HAL_ERROR;
  HAL_Delay(1);

  if (write4bits(0x20, 0) != HAL_OK) return HAL_ERROR;
  HAL_Delay(1);

  if (lcd_cmd(0x28) != HAL_OK) return HAL_ERROR;

  if (lcd_cmd(0x0C) != HAL_OK) return HAL_ERROR;

  if (lcd_cmd(0x06) != HAL_OK) return HAL_ERROR;

  if (lcd_clear() != HAL_OK) return HAL_ERROR;

  return HAL_OK;
}

/* dispInit */
HAL_StatusTypeDef dispInit(I2C_HandleTypeDef *hi2c)
{
  s_hi2c = hi2c;
  s_addr7 = 0;
  s_addr8 = 0;

  s_addr7 = autodetect_addr7();
  if (s_addr7 == 0) return HAL_ERROR;

  s_addr8 = (uint8_t)(s_addr7 << 1);

  if (lcd_init_sequence() != HAL_OK)
    return HAL_ERROR;

  s_last_state = (led_state_t)255;
  lcd_set_cursor(0, 0);
  lcd_print_padded("READY");
  return HAL_OK;
}

/* dispSetState */
void dispSetState(led_state_t state)
{
  if (s_hi2c == NULL || s_addr7 == 0) return;
  if (state == s_last_state) return;

  s_last_state = state;

  lcd_set_cursor(0, 0);

  switch (state)
  {
    case OPEN:
      lcd_print_padded("OPEN");
      break;

    case CLOSED:
      lcd_print_padded("CLOSED");
      break;

    case MOVEMENT_IN_PROGRESS:
      lcd_print_padded("IN PROGRESS");
      break;

    default:
      lcd_print_padded("UNKNOWN");
      break;
  }

  if (LCD_ROWS > 1)
  {
    lcd_set_cursor(0, 1);
    lcd_print_padded("");
  }
}

/* dispPrint2 */
void dispPrint2(const char *line0, const char *line1)
{
  if (s_hi2c == NULL || s_addr7 == 0) return;

  s_last_state = (led_state_t)255;

  lcd_set_cursor(0, 0);
  lcd_print_padded(line0 ? line0 : "");
  if (LCD_ROWS > 1)
  {
    lcd_set_cursor(0, 1);
    lcd_print_padded(line1 ? line1 : "");
  }
}

/* dispGetAddr7bit */
uint8_t dispGetAddr7bit(void)
{
  return s_addr7;
}
