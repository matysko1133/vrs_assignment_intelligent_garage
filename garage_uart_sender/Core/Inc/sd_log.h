/* sd_log.h */
#ifndef INC_SD_LOG_H_
#define INC_SD_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void SDLog_Init(void);
void SDLog_Line(const char *line);
void SDLog_Printf(const char *fmt, ...);

void SDLog_Process(uint8_t safe_to_write);

uint16_t SDLog_QueueCount(void);

void SDLog_SetDateTime(uint16_t year, uint8_t month, uint8_t day,
                       uint8_t hour, uint8_t min, uint8_t sec);

#ifdef __cplusplus
}
#endif

#endif
