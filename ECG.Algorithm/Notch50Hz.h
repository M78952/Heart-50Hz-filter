#ifndef __ECG_NOTCH50HZ_H
#define __ECG_NOTCH50HZ_H

#include <stdint.h>

/* Parameters copied from the MATLAB design. */
#define ECG_NOTCH_SAMPLE_RATE_HZ  (500.0f)  /* Fs */
#define ECG_NOTCH_CENTER_FREQ_HZ  (50.0f)   /* Fc */
#define ECG_NOTCH_BETA            (0.96f)   /* pole radius from MATLAB */

/*
 * MATLAB:
 *   alpha = -2*cos(2*pi*Fc/Fs);
 *   b = [1, alpha, 1];
 *   a = [1, alpha*beta, beta^2];
 *   dataOut = dlsim(b, a, dataIn);
 *
 * The C state variables are x[n-1], x[n-2], y[n-1], y[n-2].
 */
typedef struct
{
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
    float x1;
    float x2;
    float y1;
    float y2;
} ECG_Notch50HzFilter;

/* Calculate b/a and clear all states, matching dlsim zero initial state. */
void ECG_Notch50Hz_Init(ECG_Notch50HzFilter *filter);

/* Clear the four delay states to zero. */
void ECG_Notch50Hz_Reset(ECG_Notch50HzFilter *filter);

/* Process exactly one input sample. */
float ECG_Notch50Hz_Process(ECG_Notch50HzFilter *filter, float input);

#endif
