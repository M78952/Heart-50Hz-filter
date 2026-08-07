/*
 * 50 Hz ECG notch-filter template for STM32F103 + Standard Peripheral Library
 * ---------------------------------------------------------------------------
 * MATLAB reference (offline):
 *
 *     Fs = 500;                         % TODO: use the real ADC rate
 *     f0 = 50;                          % TODO: 50 Hz (or 60 Hz)
 *     Q  = 35;                          % TODO: try a different value
 *     wo = 2*f0/Fs;
 *     bw = wo/Q;
 *     [b,a] = iirnotch(wo,bw);
 *     y = filter(b,a,x);
 *
 * The code below is the causal, sample-by-sample equivalent: a second-order
 * IIR (biquad) notch.  It is intentionally not called from main.c yet, so the
 * existing raw ADC stream is unchanged while you fill in and test parameters.
 *
 * Difference equation:
 *
 *     y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
 *                         - a1*y[n-1] - a2*y[n-2]
 *
 * Coefficients use the common RBJ notch form:
 *
 *     w0    = 2*pi*f0/Fs
 *     alpha = sin(w0)/(2*Q)
 *     a0    = 1 + alpha
 *     b0    = 1/a0, b1 = -2*cos(w0)/a0, b2 = 1/a0
 *     a1    = -2*cos(w0)/a0, a2 = (1-alpha)/a0
 *
 * If exact coefficient-for-coefficient agreement with MATLAB iirnotch is
 * required, export MATLAB's b and a values and replace the five assignments
 * in ECG_Notch50Hz_Init().
 */

#include <math.h>
#include "Notch50Hz.h"

#ifndef ECG_NOTCH_PI
#define ECG_NOTCH_PI (3.14159265358979323846f)
#endif

void ECG_Notch50Hz_Init(ECG_Notch50HzFilter *filter)
{
    float w0;
    float alpha;
    float a0;
    float cosine;

    if (filter == (ECG_Notch50HzFilter *)0) {
        return;
    }

    /* TODO: verify Fs > 2*f0 and Q > 0 before using the filter. */
    w0 = 2.0f * ECG_NOTCH_PI * ECG_NOTCH_CENTER_FREQ_HZ /
         ECG_NOTCH_SAMPLE_RATE_HZ;
    alpha = sinf(w0) / (2.0f * ECG_NOTCH_Q);
    a0 = 1.0f + alpha;
    cosine = cosf(w0);

    filter->b0 = 1.0f / a0;
    filter->b1 = -2.0f * cosine / a0;
    filter->b2 = 1.0f / a0;
    filter->a1 = -2.0f * cosine / a0;
    filter->a2 = (1.0f - alpha) / a0;

    filter->x1 = 0.0f;
    filter->x2 = 0.0f;
    filter->y1 = 0.0f;
    filter->y2 = 0.0f;
}

float ECG_Notch50Hz_Process(ECG_Notch50HzFilter *filter, float input)
{
    float output;

    if (filter == (ECG_Notch50HzFilter *)0) {
        return input;
    }

    output = filter->b0 * input
           + filter->b1 * filter->x1
           + filter->b2 * filter->x2
           - filter->a1 * filter->y1
           - filter->a2 * filter->y2;

    filter->x2 = filter->x1;
    filter->x1 = input;
    filter->y2 = filter->y1;
    filter->y1 = output;

    return output;
}
