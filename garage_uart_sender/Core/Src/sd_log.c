/* sd_log.c */
#include "sd_log.h"
#include "main.h"
#include "fatfs.h"
#include "ff.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef struct {
  int year, month, day;
  int hour, min, sec;
} build_dt_t;

static uint8_t s_time_inited = 0;
static uint8_t s_ext_time_set = 0;
static build_dt_t s_build_dt;
static uint32_t s_boot_tick = 0;

/* is_leap */
static int is_leap(int y)
{
  return ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0);
}

/* days_in_month */
static int days_in_month(int y, int m)
{
  static const int mdays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  if (m == 2) return mdays[m-1] + (is_leap(y) ? 1 : 0);
  if (m >= 1 && m <= 12) return mdays[m-1];
  return 30;
}

/* add_seconds */
static void add_seconds(build_dt_t *t, uint32_t add)
{
  if (!t) return;

  uint32_t s = (uint32_t)t->sec + add;
  t->sec = (int)(s % 60);
  uint32_t m = (uint32_t)t->min + (s / 60);
  t->min = (int)(m % 60);
  uint32_t h = (uint32_t)t->hour + (m / 60);
  t->hour = (int)(h % 24);
  uint32_t dadd = (h / 24);

  while (dadd > 0)
  {
    int dim = days_in_month(t->year, t->month);
    if (t->day < dim)
    {
      t->day++;
      dadd--;
    }
    else
    {

      t->day = 1;
      if (t->month < 12) t->month++;
      else { t->month = 1; t->year++; }
      dadd--;
    }
  }
}

/* init_build_time_once */
static void init_build_time_once(void)
{
  if (s_time_inited) return;

  char mon_str[4] = {0};
  int day = 1, year = 2000, hh = 0, mm = 0, ss = 0;

  (void)sscanf(__DATE__, "%3s %d %d", mon_str, &day, &year);
  (void)sscanf(__TIME__, "%d:%d:%d", &hh, &mm, &ss);

  int month = 1;
  const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  const char *p = strstr(months, mon_str);
  if (p) month = (int)((p - months) / 3) + 1;

  s_build_dt.year = year;
  s_build_dt.month = month;
  s_build_dt.day = day;
  s_build_dt.hour = hh;
  s_build_dt.min = mm;
  s_build_dt.sec = ss;

  s_boot_tick = HAL_GetTick();
  s_time_inited = 1;
}

/* format_timestamp */
static void format_timestamp(char *out, size_t out_sz)
{
  if (!out || out_sz == 0) return;
  init_build_time_once();

  build_dt_t t = s_build_dt;

  uint32_t elapsed_s = (HAL_GetTick() - s_boot_tick) / 1000U;
  add_seconds(&t, elapsed_s);

  (void)snprintf(out, out_sz, "%02d.%02d.%04d %02d:%02d:%02d",
                 t.day, t.month, t.year, t.hour, t.min, t.sec);
}

static uint8_t mounted = 0;

#define LOGQ_LINES    32
#define LOGQ_LINELEN  256

static char logq[LOGQ_LINES][LOGQ_LINELEN];
static volatile uint16_t logq_head = 0;
static volatile uint16_t logq_tail = 0;

/* return */
static uint16_t logq_next(uint16_t x) { return (uint16_t)((x + 1U) % LOGQ_LINES); }

/* SDLog_QueueCount */
uint16_t SDLog_QueueCount(void)
{
  if (logq_head >= logq_tail) return (uint16_t)(logq_head - logq_tail);
  return (uint16_t)(LOGQ_LINES - (logq_tail - logq_head));
}

/* sanitize_ascii */
static void sanitize_ascii(char *s)
{
  for (; *s; s++)
  {
    unsigned char c = (unsigned char)(*s);

    if (c == '\n' || c == '\r') continue;
    if (c < 32 || c > 126) *s = ' ';
  }
}

/* logq_push */
static void logq_push(const char *line)
{
  if (!line) return;

  char tmp[LOGQ_LINELEN];

  size_t n = 0U;
  while (line[n] && n < (LOGQ_LINELEN - 3U)) { n++; }
  memcpy(tmp, line, n);
  tmp[n] = '\0';

  while (n > 0U && (tmp[n-1] == '\n' || tmp[n-1] == '\r')) { tmp[n-1] = '\0'; n--; }

  if (n < LOGQ_LINELEN - 3U)
  {
    tmp[n++] = '\n';
    tmp[n] = '\0';
  }

  sanitize_ascii(tmp);

  uint16_t next = logq_next(logq_head);
  if (next == logq_tail)
  {

    logq_tail = logq_next(logq_tail);
  }

  strncpy(logq[logq_head], tmp, LOGQ_LINELEN - 1U);
  logq[logq_head][LOGQ_LINELEN - 1U] = '\0';
  logq_head = next;
}

/* ensure_mounted */
static void ensure_mounted(void)
{
  if (mounted) return;

  FRESULT fr = f_mount(&USERFatFS, USERPath, 1);
  if (fr == FR_OK) mounted = 1;
}

/* SDLog_Init */
void SDLog_Init(void)
{
  mounted = 0;
  init_build_time_once();
  ensure_mounted();

  SDLog_Printf("SYSTEM INIT - INTELIGENTNA GARAZ");
}

/* SDLog_Line */
void SDLog_Line(const char *line)
{

  logq_push(line);
}

/* SDLog_Printf */
void SDLog_Printf(const char *fmt, ...)
{
  if (!fmt) return;

  char msg[160];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  size_t n = strlen(msg);
  while (n > 0 && (msg[n-1] == '\n' || msg[n-1] == '\r')) { msg[n-1] = '\0'; n--; }

  char ts[32];
  format_timestamp(ts, sizeof(ts));

  char line[220];

  snprintf(line, sizeof(line), "%s - %s\n", ts, msg);

  SDLog_Line(line);
}

/* SDLog_Process */
void SDLog_Process(uint8_t safe_to_write)
{
  if (!safe_to_write) return;

  ensure_mounted();
  if (!mounted) return;

  if (logq_tail == logq_head) return;

  FRESULT fr = f_open(&USERFile, "0:/LOG2.TXT", FA_OPEN_ALWAYS | FA_WRITE);
  if (fr != FR_OK) {

    if (fr == FR_NOT_READY || fr == FR_DISK_ERR || fr == FR_INT_ERR) {
      mounted = 0;
    }
    return;
  }

  f_lseek(&USERFile, f_size(&USERFile));

  while (logq_tail != logq_head)
  {
    const char *s = logq[logq_tail];
    if (!s || s[0] == '\0') { logq_tail = logq_next(logq_tail); continue; }

    if (f_puts(s, &USERFile) == EOF)
    {
      break;
    }

    logq_tail = logq_next(logq_tail);
  }

  (void)f_sync(&USERFile);
  f_close(&USERFile);
}

void SDLog_SetDateTime(uint16_t year, uint8_t month, uint8_t day,
                       uint8_t hour, uint8_t min, uint8_t sec)
{

  if (year < 2000 || year > 2099) return;
  if (month < 1 || month > 12) return;
  if (day < 1 || day > 31) return;
  if (hour > 23 || min > 59 || sec > 59) return;

  s_build_dt.year  = year;
  s_build_dt.month = month;
  s_build_dt.day   = day;
  s_build_dt.hour  = hour;
  s_build_dt.min   = min;
  s_build_dt.sec   = sec;

  s_boot_tick = HAL_GetTick();
  s_time_inited = 1;
  s_ext_time_set = 1;
}
