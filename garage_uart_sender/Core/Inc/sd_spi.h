/* sd_spi.h */
#ifndef INC_SD_SPI_H_
#define INC_SD_SPI_H_

#pragma once
#include <stdint.h>

typedef enum {
  SD_OK = 0,
  SD_ERR = -1,
  SD_TIMEOUT = -2
} sd_status_t;

sd_status_t SD_SPI_InitCard(void);
sd_status_t SD_SPI_ReadBlocks(uint8_t *buff, uint32_t lba, uint32_t count);
sd_status_t SD_SPI_WriteBlocks(const uint8_t *buff, uint32_t lba, uint32_t count);

uint8_t     SD_SPI_IsReady(void);
uint32_t    SD_SPI_GetSectorCount(void);

#endif
