#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"

#include "rcc_handler.h"

void RCC_Configuration(void)
{
	RCC_DeInit();
	
	/* Enable HSI */
	RCC_HSICmd(RCC_CR_HSION);
	while((RCC->CR & RCC_CR_HSIRDY) == 0); 	/* Wait for HSI Ready */
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	while(RCC_GetSYSCLKSource() != RCC_CFGR_SWS_HSI);	/* Wait for HSI used as system clock */
	
	FLASH->ACR  = FLASH_ACR_PRFTEN;                           /* Enable Prefetch Buffer            */
  FLASH->ACR |= FLASH_ACR_ICEN;                             /* Instruction cache enable          */
  FLASH->ACR |= FLASH_ACR_DCEN;                             /* Data cache enable                 */
  FLASH->ACR |= FLASH_ACR_LATENCY_5WS;                      /* Flash 5 wait state                */

	RCC_HCLKConfig(RCC_SYSCLK_Div1); 	/* HCLK = SYSCLK */
	
	RCC_PCLK1Config(RCC_HCLK_Div1); 			/* APB1 = HCLK/4 */
	RCC_PCLK2Config(RCC_HCLK_Div1);				/* APB2 = HCLK/2 */
	
	RCC_PLLConfig(RCC_PLLCFGR_PLLSRC_HSI, 16, 384, 8, 8);
	
	RCC_PLLCmd(ENABLE); 	/* Enable PLL */
	
	/* Wait till PLL is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ;
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	while(RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL) ;
	
	
	/* Enable GPIO clock & DMA1 clock*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA2, ENABLE);
	
	/* Enable USART#1 clock and SPI1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_SPI1, ENABLE);
	
	/* TIM2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
}
