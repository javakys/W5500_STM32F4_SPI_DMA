#ifndef _TIMER_HANDLER_H_
#define _TIMER_HANDLER_H_

#include <stdint.h>

void Timer_Configuration(void);
uint8_t GetIsTimeElapsed(void);
void SetIsTimeElapsed(uint8_t);
uint16_t GetResetTimeCounter(void);
void SetResetTimeCounter(uint16_t);

#endif
