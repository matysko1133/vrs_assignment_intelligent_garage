/* sd_spi.c */
#include "sd_spi.h"
#include "stm32f3xx_hal.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;

#define SD_CS_GPIO_Port GPIOA
#define SD_CS_Pin       GPIO_PIN_4

static uint8_t  s_ready = 0;
static uint8_t  s_sdhc  = 0;
static uint32_t s_sector_count = 0;

/* HAL_GPIO_WritePin */
static inline void CS_HIGH(void){ HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET); }
/* HAL_GPIO_WritePin */
static inline void CS_LOW(void) { HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET); }

/* spi_txrx */
static uint8_t spi_txrx(uint8_t d)
{
  uint8_t r = 0xFF;
  HAL_SPI_TransmitReceive(&hspi1, &d, &r, 1, 100);
  return r;
}

/* spi_clock_dummy */
static void spi_clock_dummy(uint32_t nbytes)
{
  CS_HIGH();
  for(uint32_t i=0;i<nbytes;i++) spi_txrx(0xFF);
}

/* wait_ready */
static int wait_ready(uint32_t timeout_ms)
{
  uint32_t t0 = HAL_GetTick();
  uint8_t b;
  do {
    b = spi_txrx(0xFF);
    if (b == 0xFF) return 0;
  } while ((HAL_GetTick() - t0) < timeout_ms);
  return -1;
}

#define CMD0   (0)
#define CMD8   (8)
#define CMD9   (9)
#define CMD10  (10)
#define CMD12  (12)
#define CMD16  (16)
#define CMD17  (17)
#define CMD18  (18)
#define CMD23  (23)
#define CMD24  (24)
#define CMD25  (25)
#define CMD55  (55)
#define CMD58  (58)
#define ACMD41 (41)

/* send_cmd */
static uint8_t send_cmd(uint8_t cmd, uint32_t arg)
{
  uint8_t crc = 0x01;
  uint8_t r1;

  if (cmd & 0x80) {
    cmd &= 0x7F;
    r1 = send_cmd(CMD55, 0);
    if (r1 > 1) return r1;
  }

  CS_LOW();
  if (wait_ready(200) != 0) { CS_HIGH(); spi_txrx(0xFF); return 0xFF; }

  spi_txrx(0x40 | cmd);
  spi_txrx((uint8_t)(arg >> 24));
  spi_txrx((uint8_t)(arg >> 16));
  spi_txrx((uint8_t)(arg >> 8));
  spi_txrx((uint8_t)(arg));

  if (cmd == CMD0) crc = 0x95;
  if (cmd == CMD8) crc = 0x87;
  spi_txrx(crc);

  for (int i=0;i<10;i++) {
    r1 = spi_txrx(0xFF);
    if (!(r1 & 0x80)) break;
  }
  return r1;
}

/* rcvr_datablock */
static int rcvr_datablock(uint8_t *buff, uint32_t btr)
{
  uint32_t t0 = HAL_GetTick();
  uint8_t token;

  do {
    token = spi_txrx(0xFF);
    if (token == 0xFE) break;
  } while ((HAL_GetTick() - t0) < 200);

  if (token != 0xFE) return -1;

  for (uint32_t i=0;i<btr;i++) buff[i] = spi_txrx(0xFF);
  spi_txrx(0xFF);
  spi_txrx(0xFF);
  return 0;
}

/* xmit_datablock */
static int xmit_datablock(const uint8_t *buff, uint8_t token)
{
  if (wait_ready(500) != 0) return -1;

  spi_txrx(token);
  if (token != 0xFD) {
    for (uint32_t i=0;i<512;i++) spi_txrx(buff[i]);
    spi_txrx(0xFF); spi_txrx(0xFF);
    uint8_t resp = spi_txrx(0xFF);
    if ((resp & 0x1F) != 0x05) return -1;
  }
  return 0;
}

/* SD_SPI_IsReady */
uint8_t SD_SPI_IsReady(void){ return s_ready; }
/* SD_SPI_GetSectorCount */
uint32_t SD_SPI_GetSectorCount(void){ return s_sector_count; }

/* SD_SPI_InitCard */
sd_status_t SD_SPI_InitCard(void)
{
  s_ready = 0;
  s_sdhc  = 0;

  spi_clock_dummy(10);

  uint8_t r = send_cmd(CMD0, 0);
  CS_HIGH(); spi_txrx(0xFF);
  if (r != 0x01) return SD_ERR;

  r = send_cmd(CMD8, 0x1AA);
  uint8_t ocr[4] = {0};

  if (r == 0x01) {

    ocr[0]=spi_txrx(0xFF); ocr[1]=spi_txrx(0xFF); ocr[2]=spi_txrx(0xFF); ocr[3]=spi_txrx(0xFF);
    CS_HIGH(); spi_txrx(0xFF);

    if (ocr[2] != 0x01 || ocr[3] != 0xAA) return SD_ERR;

    uint32_t t0 = HAL_GetTick();
    do {
      r = send_cmd(0x80 | ACMD41, 1UL<<30);
      CS_HIGH(); spi_txrx(0xFF);
      if (r == 0x00) break;
    } while ((HAL_GetTick() - t0) < 1500);

    if (r != 0x00) return SD_TIMEOUT;

    r = send_cmd(CMD58, 0);
    ocr[0]=spi_txrx(0xFF); ocr[1]=spi_txrx(0xFF); ocr[2]=spi_txrx(0xFF); ocr[3]=spi_txrx(0xFF);
    CS_HIGH(); spi_txrx(0xFF);
    if (r != 0x00) return SD_ERR;

    s_sdhc = (ocr[0] & 0x40) ? 1 : 0;
  }
  else {

    CS_HIGH(); spi_txrx(0xFF);
    uint32_t t0 = HAL_GetTick();
    do {
      r = send_cmd(0x80 | ACMD41, 0);
      CS_HIGH(); spi_txrx(0xFF);
      if (r == 0x00) break;
    } while ((HAL_GetTick() - t0) < 1500);

    if (r != 0x00) return SD_TIMEOUT;

    r = send_cmd(CMD16, 512);
    CS_HIGH(); spi_txrx(0xFF);
    if (r != 0x00) return SD_ERR;
    s_sdhc = 0;
  }

  s_ready = 1;
  return SD_OK;
}

/* SD_SPI_ReadBlocks */
sd_status_t SD_SPI_ReadBlocks(uint8_t *buff, uint32_t lba, uint32_t count)
{
  if (!s_ready) return SD_ERR;

  uint32_t addr = s_sdhc ? lba : (lba * 512UL);

  if (count == 1) {
    uint8_t r = send_cmd(CMD17, addr);
    if (r != 0x00) { CS_HIGH(); spi_txrx(0xFF); return SD_ERR; }
    int ok = rcvr_datablock(buff, 512);
    CS_HIGH(); spi_txrx(0xFF);
    return (ok==0) ? SD_OK : SD_ERR;
  } else {
    uint8_t r = send_cmd(CMD18, addr);
    if (r != 0x00) { CS_HIGH(); spi_txrx(0xFF); return SD_ERR; }
    for (uint32_t i=0;i<count;i++) {
      if (rcvr_datablock(buff + i*512, 512) != 0) { break; }
    }
    send_cmd(CMD12, 0);
    CS_HIGH(); spi_txrx(0xFF);
    return SD_OK;
  }
}

/* SD_SPI_WriteBlocks */
sd_status_t SD_SPI_WriteBlocks(const uint8_t *buff, uint32_t lba, uint32_t count)
{
  if (!s_ready) return SD_ERR;

  uint32_t addr = s_sdhc ? lba : (lba * 512UL);

  if (count == 1) {
    uint8_t r = send_cmd(CMD24, addr);
    if (r != 0x00) { CS_HIGH(); spi_txrx(0xFF); return SD_ERR; }
    int ok = xmit_datablock(buff, 0xFE);
    CS_HIGH(); spi_txrx(0xFF);
    return (ok==0) ? SD_OK : SD_ERR;
  } else {

    send_cmd(0x80 | CMD23, count);
    CS_HIGH(); spi_txrx(0xFF);

    uint8_t r = send_cmd(CMD25, addr);
    if (r != 0x00) { CS_HIGH(); spi_txrx(0xFF); return SD_ERR; }

    for (uint32_t i=0;i<count;i++) {
      if (xmit_datablock(buff + i*512, 0xFC) != 0) { CS_HIGH(); spi_txrx(0xFF); return SD_ERR; }
    }
    xmit_datablock(0, 0xFD);
    CS_HIGH(); spi_txrx(0xFF);
    return SD_OK;
  }
}
