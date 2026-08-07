#include "stm32f10x.h"                  // Device header
#include "App.h"




static volatile uint16_t ADC_Buffer[ADC_BUFFER_SIZE];
static volatile uint16_t ADC_WriteIndex;
static volatile uint16_t ADC_ReadIndex;
static volatile uint32_t ADC_OverrunCount;


static void USART1_SendByte(uint8_t data)

{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {
    }
    USART_SendData(USART1, data);
}

static void USART1_SendString(const char *text)
{
    while (*text != '\0') {
        USART1_SendByte((uint8_t)*text++);
    }

    /* Wait until the final byte has completely left the USART. */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {
    }
}

static void USART1_SendUInt32(uint32_t value)
{
    char buffer[10];
    uint8_t count = 0U;

    do {
        buffer[count++] = (char)('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U);

    while (count != 0U) {
        USART1_SendByte((uint8_t)buffer[--count]);
    }
}


void ADC1_2_IRQHandler(void)
{
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
        uint16_t next_index;
        uint16_t sample = ADC_GetConversionValue(ADC1);

        next_index = (uint16_t)((ADC_WriteIndex + 1U) % ADC_BUFFER_SIZE);

        if (next_index != ADC_ReadIndex) {
            ADC_Buffer[ADC_WriteIndex] = sample;
            ADC_WriteIndex = next_index;
        } else {
            ADC_OverrunCount++;
        }

        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}

uint8_t ECG_GetSample(uint16_t *value)
{
    uint16_t read_index = ADC_ReadIndex;

    if (read_index == ADC_WriteIndex) {
        return 0U;
    }

    *value = ADC_Buffer[read_index];
    ADC_ReadIndex = (uint16_t)((read_index + 1U) % ADC_BUFFER_SIZE);
    return 1U;
}

void ECG_SendSample(uint16_t value)
{
    /* One ADC value per line, for example: 2063\r\n */
    USART1_SendUInt32(value);
    USART1_SendString("\r\n");
}
