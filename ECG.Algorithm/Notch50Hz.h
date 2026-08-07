#ifndef __ECG_NOTCH50HZ_H
#define __ECG_NOTCH50HZ_H

#include <stdint.h>

/*
 * 50 Hz notch-filter parameters.
 *
 * TODO: change these three values to match the actual ADC sampling setup and
 * the interference frequency you want to remove.  They are deliberately kept
 * here so that they can be changed without touching the filter algorithm.
 */
#define ECG_NOTCH_SAMPLE_RATE_HZ  (500.0f)  /* Fs: ADC sample rate          */
#define ECG_NOTCH_CENTER_FREQ_HZ  (50.0f)   /* f0: mains/interference        */
#define ECG_NOTCH_Q               (35.0f)   /* Q: bandwidth; larger is narrower */

typedef struct
{
    /* Normalized biquad coefficients. */
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;

    /* Two previous input samples and two previous output samples. */
    float x1;
    float x2;
    float y1;
    float y2;
} ECG_Notch50HzFilter;

/* Calculate coefficients and clear the filter history. */
void ECG_Notch50Hz_Init(ECG_Notch50HzFilter *filter);

/* Process one sample.  Call once for every ADC sample. */
float ECG_Notch50Hz_Process(ECG_Notch50HzFilter *filter, float input);

#endif
