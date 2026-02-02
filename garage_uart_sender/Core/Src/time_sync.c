/* time_sync.c */
#include "time_sync.h"
#include <string.h>
#include <stdio.h>

#include "sd_log.h"
#include "usart.h"

#define TIMESYNC_RETRY_MS   5000u
#define TIMESYNC_MAX_RETRIES 6u

static volatile uint8_t g_synced = 0;
static volatile uint8_t g_rx_byte = 0;

static char g_line[64];
static uint8_t g_line_len = 0;

static uint32_t g_last_req_ms = 0;
static uint8_t g_retries = 0;

/* timesync_send_request */
static void timesync_send_request(void)
{
  const char *msg = "TIME?\r\n";
  (void)HAL_UART_Transmit(&huart1, (uint8_t*)msg, (uint16_t)strlen(msg), 100);
  g_last_req_ms = HAL_GetTick();
  if (g_retries < 255) g_retries++;
}

/* parse_uint2 */
static int parse_uint2(const char *p)
{
  if (p[0] < '0' || p[0] > '9' || p[1] < '0' || p[1] > '9') return -1;
  return (p[0]-'0')*10 + (p[1]-'0');
}

static int parse_time_line(const char *s,
                           uint16_t *y, uint8_t *mo, uint8_t *d,
                           uint8_t *hh, uint8_t *mm, uint8_t *ss)
{

  while (*s == ' ' || *s == '\t') s++;
  if (strncmp(s, "TIME", 4) != 0) return 0;
  s += 4;
  while (*s == ' ' || *s == '\t') s++;

  if (!(s[0]>='0' && s[0]<='9')) return 0;
  int year = 0;
  for (int i = 0; i < 4; i++) {
    if (s[i] < '0' || s[i] > '9') return 0;
    year = year*10 + (s[i]-'0');
  }
  if (s[4] != '-') return 0;
  int m = parse_uint2(&s[5]);
  if (m < 1 || m > 12) return 0;
  if (s[7] != '-') return 0;
  int day = parse_uint2(&s[8]);
  if (day < 1 || day > 31) return 0;
  if (s[10] != ' ' && s[10] != 'T') return 0;

  int h = parse_uint2(&s[11]);
  if (h < 0 || h > 23) return 0;
  if (s[13] != ':') return 0;
  int mi = parse_uint2(&s[14]);
  if (mi < 0 || mi > 59) return 0;
  if (s[16] != ':') return 0;
  int se = parse_uint2(&s[17]);
  if (se < 0 || se > 59) return 0;

  *y = (uint16_t)year;
  *mo = (uint8_t)m;
  *d  = (uint8_t)day;
  *hh = (uint8_t)h;
  *mm = (uint8_t)mi;
  *ss = (uint8_t)se;
  return 1;
}

/* TimeSync_Init */
void TimeSync_Init(void)
{
  g_synced = 0;
  g_line_len = 0;
  g_last_req_ms = 0;
  g_retries = 0;

  (void)HAL_UART_Receive_IT(&huart1, (uint8_t*)&g_rx_byte, 1);

  timesync_send_request();
}

/* TimeSync_Task */
void TimeSync_Task(void)
{
  if (g_synced) return;

  if (g_retries < TIMESYNC_MAX_RETRIES) {
    uint32_t now = HAL_GetTick();
    if ((now - g_last_req_ms) >= TIMESYNC_RETRY_MS) {
      timesync_send_request();
    }
  }
}

/* TimeSync_UART_RxCpltCallback */
void TimeSync_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == NULL || huart->Instance != USART1) {
    return;
  }

  uint8_t b = g_rx_byte;

  if (!g_synced) {

    if (b == '\r') {

    } else if (b == '\n') {
      g_line[g_line_len] = '\0';

      uint16_t y; uint8_t mo, d, hh, mm, ss;
      if (parse_time_line(g_line, &y, &mo, &d, &hh, &mm, &ss)) {
        SDLog_SetDateTime(y, mo, d, hh, mm, ss);
        g_synced = 1;

        const char ok[] = "TIME OK\r\n";
        (void)HAL_UART_Transmit(&huart1, (uint8_t*)ok, (uint16_t)strlen(ok), 100);
      }

      g_line_len = 0;
    } else {
      if (g_line_len < (sizeof(g_line) - 1u)) {
        g_line[g_line_len++] = (char)b;
      } else {

        g_line_len = 0;
      }
    }
  }

  (void)HAL_UART_Receive_IT(&huart1, &g_rx_byte, 1);
}

/* TimeSync_IsSynced */
uint8_t TimeSync_IsSynced(void)
{
  return g_synced;
}
