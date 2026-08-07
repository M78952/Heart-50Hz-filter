#ifndef __INIT_H
#define __INIT_H

#include <stdint.h>

void USART1_Init(uint32_t baud_rate);
void ECG_ADC1_Init(void);
void ECG_TIM3_Init(void);

#endif

