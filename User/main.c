#include "stm32f10x.h"
#include "Init.h"
#include "App.h"

/* PA1 is connected to the ECG board's ECG analogue output. */
/*
 * Standard 72 MHz clock configuration:
 * TIM3 clock / 7200 / 20 = 500 Hz.
 * TIM3 update events are routed directly to the ADC trigger input.
 */
/* Called by the high-density startup file when ADC1 finishes a conversion. */

int main(void)
{
    uint16_t adc_value;

    USART1_Init(115200U);
    ECG_ADC1_Init();
    ECG_TIM3_Init();

    while (1) 
		{
      if (ECG_GetSample(&adc_value) != 0U) 					
			{
         ECG_SendSample(adc_value);
      }
    }
}
