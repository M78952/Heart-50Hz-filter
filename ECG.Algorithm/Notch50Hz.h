#ifndef __ECG_NOTCH50HZ_H
#define __ECG_NOTCH50HZ_H

#include <stdint.h>

/*
 * 50 Hz notch-filter parameters.
 *
 * The project samples at 500 Hz.  Q=8 gives a bandwidth of about 6.25 Hz,
 * which covers normal mains-frequency drift and settles much faster than the
 * previous Q=35 setting after a sharp ECG transition.
 */
#define ECG_NOTCH_SAMPLE_RATE_HZ  (500.0f)  /* Fs: ADC sample rate          */
#define ECG_NOTCH_CENTER_FREQ_HZ  (50.0f)   /* f0: mains/interference        */
#define ECG_NOTCH_Q               (8.0f)    /* bandwidth = f0/Q = 6.25 Hz    */

typedef struct
{
    /* Normalized biquad coefficients. */
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;

    /* Transposed Direct Form II states. */
    float state1;
    float state2;
    uint8_t primed;
} ECG_Notch50HzFilter;

/* Calculate coefficients and clear the filter history. */
void ECG_Notch50Hz_Init(ECG_Notch50HzFilter *filter);

/* Prime the internal states around the current ADC level without a step. */
void ECG_Notch50Hz_Reset(ECG_Notch50HzFilter *filter, float input);

/* Process one sample.  Call once for every ADC sample. */
float ECG_Notch50Hz_Process(ECG_Notch50HzFilter *filter, float input);

#endif
