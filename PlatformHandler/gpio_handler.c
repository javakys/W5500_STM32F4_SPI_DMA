#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#include "gpio_handler.h"
#include "main.h"

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure USART Tx, Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = USART1_TX | USART1_RX ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WIZ_SCLK | WIZ_MISO | WIZ_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure WIZ550io RDY pin */
	GPIO_InitStructure.GPIO_Pin = WIZ_RDY;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure SPI1 NSS pin */
	GPIO_InitStructure.GPIO_Pin = WIZ_SCS | WIZ_nRST;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, WIZ_SCS);
	GPIO_SetBits(GPIOA, WIZ_nRST);
	
	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	/* Connect PXx to USARTx_Rx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* Connect SPI pins to AF_SPI1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); //SCLK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); //MOSI
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); //MISO
}
