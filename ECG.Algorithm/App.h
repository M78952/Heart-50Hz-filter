#ifndef __APP_H
#define __APP_H
#include <stdint.h>
#define ECG_SAMPLE_RATE_HZ  500U
#define ADC_BUFFER_SIZE     128U


void USART1_SendByte(uint8_t data);
void USART1_SendString(const char *text);
void USART1_SendUInt32(uint32_t value);

uint8_t ECG_GetSample(uint16_t *value);
void ECG_SendSample(uint16_t value);


#endif

