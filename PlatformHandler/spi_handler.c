#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"

#include "spi_handler.h"
#include "common.h"
#include <string.h>

DMA_InitTypeDef DMA_InitStructure;
uint8_t pTmpBuf1[DATA_BUF_SIZE + 3];
uint8_t pTmpBuf2[DATA_BUF_SIZE + 3];

void SPI_Configuration(void)
{
	SPI_InitTypeDef	SPI_InitStructure;
	
	/* SPI Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI1, &SPI_InitStructure);

#if !defined (SPI_DMA)	
	SPI_Cmd(SPI1, ENABLE);
#endif
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) ;
	
}

uint8_t SPI_SendByte(uint8_t byte)
{
	SPI_I2S_SendData(SPI1, byte);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	return SPI_I2S_ReceiveData(SPI1);
}

uint8_t SPI_ReceiveByte(void)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) ;

	  SPI_I2S_SendData(SPI1, 0);

	  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	  return SPI_I2S_ReceiveData(SPI1);
}

void SPI_SendBurst(uint8_t* pBuf, uint16_t len)
{
}

void SPI_ReceiveBurst(uint8_t* pBuf, uint16_t len)
{
}

void SPI_DMA_WRITE(uint8_t* Addref, uint8_t* pTxBuf, uint16_t tx_len)
{
	uint16_t i;

//	for(i=0; i<(tx_len+3); i++)
//		pTmpBuf1[i] = 0;

	memset(pTmpBuf1, 0, tx_len + 3);

	pTmpBuf1[0] = Addref[0];
	pTmpBuf1[1] = Addref[1];
	pTmpBuf1[2] = Addref[2];

	for(i=0; i<tx_len; i++)
		pTmpBuf1[3 + i] = pTxBuf[i];
//	DMA_InitTypeDef DMA_InitStructure;

	DMA_InitStructure.DMA_BufferSize = (uint16_t)(tx_len + 3);
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DR));
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize =  DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	/* Configure Tx DMA */
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) pTmpBuf1;
	DMA_Init(DMA2_Stream3, &DMA_InitStructure);

/* Configure Rx DMA */
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) pTmpBuf1;
	DMA_Init(DMA2_Stream2, &DMA_InitStructure);




	/* Enable the DMA channel */

	wizchip_select();

	/* Enable the DMA SPI TX Stream */
	DMA_Cmd(DMA2_Stream3, ENABLE);
	/* Enable the DMA SPI RX Stream */
	DMA_Cmd(DMA2_Stream2, ENABLE);

	/* Enable the SPI Rx DMA request */
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

	 SPI_Cmd(SPI1, ENABLE);
	 
  /* Waiting the end of Data transfer */
  while (DMA_GetFlagStatus(DMA2_Stream3, DMA_FLAG_TCIF3)==RESET);
  while (DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2)==RESET);

//	printf("DMA_WRITE after select\r\n");
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
//	{}


//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
//	{}

	wizchip_deselect();

//	printf("DMA_WRITE after deselect\r\n");
//	while((DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET)){}

//	printf("2\r\n");
	DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);

	DMA_Cmd(DMA2_Stream3, DISABLE);
	DMA_Cmd(DMA2_Stream2, DISABLE);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, DISABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
	
	SPI_Cmd(SPI1, DISABLE);
}

void SPI_DMA_READ(uint8_t* Addref, uint8_t* pRxBuf, uint16_t rx_len)
{
	uint16_t i;

//	for(i=0; i<(rx_len + 3); i++)
//	{
//		pTmpBuf1[i] = pTmpBuf2[i] = 0;
//	}

	memset(pTmpBuf1, 0, rx_len + 3);
	memset(pTmpBuf2, 0, rx_len + 3);

	pTmpBuf1[0] = Addref[0];
	pTmpBuf1[1] = Addref[1];
	pTmpBuf1[2] = Addref[2];

#if 0
	printf("rx len: %d\r\n", rx_len);
	printf("Rx Data: ");
	for(i=0; i<rx_len; i++)
		printf("%02X ", pRxBuf[i]);
	printf("\r\n");

	printf("pTmpBuf1: ");
	for(i=0; i<rx_len + 3; i++)
		printf("%02X ", pTmpBuf1[i]);
	printf("\r\n");
	

#endif

	DMA_InitStructure.DMA_BufferSize = (uint16_t)(rx_len + 3);
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DR));
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize =  DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	/* Configure Tx DMA */
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) pTmpBuf1;
	DMA_Init(DMA2_Stream3, &DMA_InitStructure);

/* Configure Rx DMA */
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) pTmpBuf2;
	DMA_Init(DMA2_Stream2, &DMA_InitStructure);


	/* Enable the DMA channel */

	/* Enable the SPI Rx DMA request */
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

	wizchip_select();

	/* Enable the DMA SPI TX Stream */
	DMA_Cmd(DMA2_Stream3, ENABLE);
	/* Enable the DMA SPI RX Stream */
	DMA_Cmd(DMA2_Stream2, ENABLE);


  SPI_Cmd(SPI1, ENABLE);
  /* Waiting the end of Data transfer */
  while (DMA_GetFlagStatus(DMA2_Stream3, DMA_FLAG_TCIF3)==RESET);
  while (DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2)==RESET);

//	printf("DMA_READ after select\r\n");
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
//	{}


//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
//	{}

	wizchip_deselect();

//	printf("DMA_READ after deselect\r\n");
//	while((DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET)){}

//	printf("2\r\n");
	DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);

	DMA_Cmd(DMA2_Stream3, DISABLE);
	DMA_Cmd(DMA2_Stream2, DISABLE);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, DISABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);

	SPI_Cmd(SPI1, DISABLE);
//	for(i=0; i<rx_len; i++)
//		pRxBuf[i] = pTmpBuf2[3 + i];

	memcpy(pRxBuf, pTmpBuf2 + 3, rx_len);

#if 0
	printf("Rx Data: ");
	for(i=0; i<rx_len; i++)
		printf("%02X ", pRxBuf[i]);
	printf("\r\n");
#endif
}
