#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

#include "timer_handler.h"

uint8_t IsTimeElapsed = 0;
uint16_t TimerCount = 0;
uint16_t ResetTimeCounter = 0;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

void Timer_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 1000;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_PrescalerConfig(TIM2, 47, TIM_PSCReloadMode_Immediate);
	
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		if(TimerCount++ >= 1000)
		{
			TimerCount = 0;
			IsTimeElapsed = 1;
		}
		
		if(ResetTimeCounter++ < 100)
			;
	}
}

uint8_t GetIsTimeElapsed(void)
{
	return IsTimeElapsed;
}

void SetIsTimeElapsed(uint8_t flag)
{
	IsTimeElapsed = flag;
}

uint16_t GetResetTimeCounter(void)
{
	return ResetTimeCounter;
}

void SetResetTimeCounter(uint16_t count)
{
	ResetTimeCounter = count;
}
