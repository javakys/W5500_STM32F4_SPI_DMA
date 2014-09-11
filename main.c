/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-January-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "Ethernet\wizchip_conf.h"
#include "Application\loopback.h"
#include "PlatformHandler\rcc_handler.h"
#include "PlatformHandler\gpio_handler.h"
#include "PlatformHandler\timer_handler.h"
#include "PlatformHandler\uart_handler.h"
#include "PlatformHandler\spi_handler.h"
#include "PlatformHandler\dma_handler.h"
#include "common.h"

/** @addtogroup STM32F4xx_StdPeriph_Templates
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#if defined (USE_STM324xG_EVAL)
  #define MESSAGE1   "     STM32F40xx     "
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM324xG-EVAL    "

#else /* USE_STM324x7I_EVAL */ 
  #define MESSAGE1   "     STM32F427x     "
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "  STM324x7I-EVAL    "
#endif 



#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef    RCC_Clocks;


RCC_ClocksTypeDef RCC_ClockFreq;

uint8_t gDATABUF[DATA_BUF_SIZE];
///////////////////////////////////
// Default Network Configuration //
///////////////////////////////////
wiz_NetInfo gWIZNETINFO = {{0x00, 0x08, 0xdc, 0x00, 0xab, 0xcd},
																							{192, 168, 1, 123},
																							{255,255,255,0},
																							{192, 168, 1, 1},
																							{8,8,8,8},
																							NETINFO_STATIC };


/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  This function handles TIM2 Handler.
  * @param  None
  * @retval None
  */

void test_putch(char ch)
{
	while(!(USART1->SR & 0x0080));
	USART1->DR = (ch & 0xFF);
}



void  wizchip_select(void)
{
	GPIO_ResetBits(GPIOA, WIZ_SCS);
}

void  wizchip_deselect(void)
{
	GPIO_SetBits(GPIOA, WIZ_SCS);
}

void  wizchip_write(uint8_t wb)
{
	SPI_SendByte(wb);
}

uint8_t wizchip_read()
{
	return SPI_ReceiveByte();
}



void network_init(void)
{
   uint8_t tmpstr[6];

	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);

	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("======================\r\n");

#if 1
    gWIZNETINFO.ip[0] = 222;
    gWIZNETINFO.ip[1] = 98;
    gWIZNETINFO.ip[2] = 173;
    gWIZNETINFO.ip[3] = 239;
    gWIZNETINFO.gw[0] = 222;
    gWIZNETINFO.gw[1] = 98;
    gWIZNETINFO.gw[2] = 173;
    gWIZNETINFO.gw[3] = 254;
    gWIZNETINFO.sn[0] = 255;
    gWIZNETINFO.sn[1] = 255;
    gWIZNETINFO.sn[2] = 255;
    gWIZNETINFO.sn[3] = 192;
    gWIZNETINFO.dns[0] = 8;
    gWIZNETINFO.dns[1] = 8;
    gWIZNETINFO.dns[2] = 8;
    gWIZNETINFO.dns[3] = 8;
    gWIZNETINFO.dhcp = NETINFO_STATIC;
#endif

    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);


	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("======================\r\n");
}
/////////////////////////////////////////////////////////////

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40xx.s/startup_stm32f427x.s) before to branch to 
       application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */  
	uint32_t current_time = 0;
	uint8_t tmp = 0;
	uint8_t ret = 0;
	uint8_t sn = 0;

	 uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2}, {2,2,2,2,2,2,2,2}};

	RCC_Configuration();
	GPIO_Configuration();
	Timer_Configuration();
	DMA_Configuration();
  SPI_Configuration();
	USART_Configuration();  /* SysTick end of count event each 10ms */
  
//	RCC_GetClocksFreq(&RCC_Clocks);
 // SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  

  /* Add your application code here */

	


/* Infinite loop */
	

		
		 RCC_GetClocksFreq(&RCC_ClockFreq);
		 
		 printf("SYSCLK frequency: %d\r\n", RCC_ClockFreq.SYSCLK_Frequency);
		 printf("PCLK1 frequency: %d\r\n", RCC_ClockFreq.PCLK1_Frequency);
		 printf("PCLK2 frequency: %d\r\n", RCC_ClockFreq.PCLK2_Frequency);
		 
		 reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
		 
		 /* SPI read & write callback function */
		 reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	
		 GPIO_ResetBits(GPIOA, WIZ_nRST);
		 SetResetTimeCounter(0);
		 
		 while(1)
		 {
			 if(GetResetTimeCounter() >= 100)
				 break;
		 }
		 GPIO_SetBits(GPIOA, WIZ_nRST);
		 
		 while(1)
		 {
			 if(GPIO_ReadInputDataBit(GPIOA, WIZ_RDY) == 1)
				 break;
		 }

    /* WIZCHIP SOCKET Buffer initialize */
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
       printf("WIZCHIP Initialized fail.\r\n");
       while(1);
    }

    /* PHY link status check */
    do
    {
       if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
          printf("Unknown PHY Link status.\r\n");
    }while(tmp == PHY_LINK_OFF);

		
	printf("W5500 Demo on STM32F401RE Cortex M0\r\n");

	/* Network Initialization */
	network_init();
		
  while (1)
  {

#if 0		
		if(GetIsTimeElapsed())
		{
			SetIsTimeElapsed(0);
			printf("Time Elapsed\r\n");
		}
#endif
		
#if 1		
		if((ret = loopback_tcps(sn, gDATABUF, 3000)) < 0)
		{
			printf("%d:loopback_tcps error:%d\r\n", sn, ret);
			break;
		}
		if((ret = loopback_tcps(sn+1, gDATABUF, 3000)) < 0)
		{
			printf("%d:loopback_tcps error:%d\r\n", sn, ret);
			break;
		}
 #endif
#if 0			
		if((ret = rcvonly_tcps(sn, gDATABUF, 3000)) < 0)
		{
			printf("%d:loopback_tcps error:%d\r\n", sn, ret);
			break;
		}
 #endif
	}
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
