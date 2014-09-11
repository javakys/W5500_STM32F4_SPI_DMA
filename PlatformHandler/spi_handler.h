#ifndef __SPI_HANDLER_H
#define __SPI_HANDLER_H

//#include "stm32f4xx.h"

typedef unsigned char	uint8_t;
typedef unsigned short uint16_t;

#ifndef SPI_DMA
#define SPI_DMA
#endif

void SPI_Configuration(void);

uint8_t SPI_SendByte(uint8_t byte);
uint8_t SPI_ReceiveByte(void);

void SPI_SendBurst(uint8_t* pBuf, uint16_t len);
void SPI_ReceiveBurst(uint8_t* pBuf, uint16_t len);

void SPI_DMA_WRITE(uint8_t* Addref, uint8_t* pTxBuf, uint16_t tx_len);
void SPI_DMA_READ(uint8_t* Addref, uint8_t* pRxBuf, uint16_t rx_len);

#endif
