#ifndef __ECG_NOTCH50HZ_H
#define __ECG_NOTCH50HZ_H

#include <stdint.h>

/*
 * 50 Hz notch-filter parameters.
 *
 * Recommended comparison:
 *   order 2: one Q=4 section, shortest ringing (default)
 *   order 4: two Q=6 sections, stronger rejection around 49...51 Hz
 *
 * Change ECG_NOTCH_ORDER to 4U only when you want to test the fourth-order
 * version.  Both modes are built from stable second-order sections.
 */
#define ECG_NOTCH_SAMPLE_RATE_HZ  (500.0f)  /* Fs: ADC sample rate          */
#define ECG_NOTCH_CENTER_FREQ_HZ  (50.0f)   /* f0: mains/interference        */
#define ECG_NOTCH_ORDER           (2U)      /* choose 2U or 4U              */

#if (ECG_NOTCH_ORDER == 2U)
#define ECG_NOTCH_STAGE_COUNT     (1U)
#define ECG_NOTCH_Q               (4.0f)    /* bandwidth = 12.5 Hz           */
#elif (ECG_NOTCH_ORDER == 4U)
#define ECG_NOTCH_STAGE_COUNT     (2U)
#define ECG_NOTCH_Q               (6.0f)    /* Q for each cascaded section   */
#else
#error "ECG_NOTCH_ORDER must be 2U or 4U"
#endif

typedef struct
{
    /* Normalized biquad coefficients. */
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;

    /* One pair of Transposed Direct Form II states for each biquad. */
    float state1[ECG_NOTCH_STAGE_COUNT];
    float state2[ECG_NOTCH_STAGE_COUNT];
    uint8_t primed;
} ECG_Notch50HzFilter;

/* Calculate coefficients and clear the filter history. */
void ECG_Notch50Hz_Init(ECG_Notch50HzFilter *filter);

/* Prime the internal states around the current ADC level without a step. */
void ECG_Notch50Hz_Reset(ECG_Notch50HzFilter *filter, float input);

/* Process one sample.  Call once for every ADC sample. */
float ECG_Notch50Hz_Process(ECG_Notch50HzFilter *filter, float input);

#endif
