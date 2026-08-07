#include "stm32f10x.h"
#include "Init.h"
#include "App.h"
#include "Notch50Hz.h"

#define ECG_ADC_MAX_VALUE (4095.0f)

/* Convert the floating-point filter output back to a 12-bit ADC value. */
static uint16_t ECG_FilteredValueToAdc(float value)
{
    if (value <= 0.0f) {
        return 0U;
    }

    if (value >= ECG_ADC_MAX_VALUE) {
        return 4095U;
    }

    /* Round to the nearest integer instead of truncating. */
    return (uint16_t)(value + 0.5f);
}

int main(void)
{
    uint16_t adc_value;
    uint16_t filtered_adc_value;
    float filtered_value;
    ECG_Notch50HzFilter notch_filter;

    USART1_Init(115200U);
    ECG_ADC1_Init();
    ECG_TIM3_Init();
    ECG_Notch50Hz_Init(&notch_filter);

    while (1) {
        if (ECG_GetSample(&adc_value) != 0U) {
            /* One causal IIR step for each ADC sample. */
            filtered_value = ECG_Notch50Hz_Process(&notch_filter,
                                                   (float)adc_value);
            filtered_adc_value = ECG_FilteredValueToAdc(filtered_value);

            /* UART output is now the filtered ADC value, one line per sample. */
            ECG_SendSample(filtered_adc_value);
        }
    }
}
