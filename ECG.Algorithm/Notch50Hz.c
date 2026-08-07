/*
 * 50 Hz ECG notch-filter template for STM32F103 + Standard Peripheral Library
 * ---------------------------------------------------------------------------
 * MATLAB reference (offline):
 *
 *     Fs = 500;                         % TODO: use the real ADC rate
 *     f0 = 50;                          % TODO: 50 Hz (or 60 Hz)
 *     Q  = 4;                           % default second-order mode
 *     wo = 2*f0/Fs;
 *     bw = wo/Q;
 *     [b,a] = iirnotch(wo,bw);
 *     y = filter(b,a,x);
 *
 * The code below is the causal, sample-by-sample equivalent: a second-order
 * IIR (biquad) notch.  main.c calls ECG_Notch50Hz_Process() once for every
 * ADC sample, then sends the filtered value over USART1.
 *
 * Transposed Direct Form II equations:
 *
 *     y  = b0*x + state1
 *     state1 = b1*x - a1*y + state2
 *     state2 = b2*x - a2*y
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

static void ECG_Notch50Hz_ClearState(ECG_Notch50HzFilter *filter)
{
    uint8_t stage;

    for (stage = 0U; stage < ECG_NOTCH_STAGE_COUNT; stage++) {
        filter->state1[stage] = 0.0f;
        filter->state2[stage] = 0.0f;
    }

    filter->primed = 0U;
}

void ECG_Notch50Hz_Init(ECG_Notch50HzFilter *filter)
{
    float w0;
    float alpha;
    float a0;
    float cosine;

    if (filter == (ECG_Notch50HzFilter *)0) {
        return;
    }

    /*
     * A digital notch must satisfy 0 < f0 < Fs/2 and Q > 0.  If a parameter
     * is entered incorrectly, use a unity/bypass filter instead of dividing
     * by zero or generating unstable coefficients.
     */
    if ((ECG_NOTCH_SAMPLE_RATE_HZ <= 0.0f) ||
        (ECG_NOTCH_CENTER_FREQ_HZ <= 0.0f) ||
        (ECG_NOTCH_CENTER_FREQ_HZ >=
         (0.5f * ECG_NOTCH_SAMPLE_RATE_HZ)) ||
        (ECG_NOTCH_Q <= 0.0f)) {
        filter->b0 = 1.0f;
        filter->b1 = 0.0f;
        filter->b2 = 0.0f;
        filter->a1 = 0.0f;
        filter->a2 = 0.0f;
        ECG_Notch50Hz_ClearState(filter);
        return;
    }

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

    ECG_Notch50Hz_ClearState(filter);
}

void ECG_Notch50Hz_Reset(ECG_Notch50HzFilter *filter, float input)
{
    uint8_t stage;

    if (filter == (ECG_Notch50HzFilter *)0) {
        return;
    }

    /*
     * Initialize the two states to the steady-state values for a constant
     * input.  The notch has unity DC gain, so its first output equals input
     * instead of treating the ADC midpoint as a large step from zero.
     */
    for (stage = 0U; stage < ECG_NOTCH_STAGE_COUNT; stage++) {
        filter->state1[stage] = (1.0f - filter->b0) * input;
        filter->state2[stage] = (filter->b2 - filter->a2) * input;
    }

    filter->primed = 1U;
}

float ECG_Notch50Hz_Process(ECG_Notch50HzFilter *filter, float input)
{
    float output;
    float stage_input;
    uint8_t stage;

    if (filter == (ECG_Notch50HzFilter *)0) {
        return input;
    }

    if (filter->primed == 0U) {
        ECG_Notch50Hz_Reset(filter, input);
        return input;
    }

    stage_input = input;

    for (stage = 0U; stage < ECG_NOTCH_STAGE_COUNT; stage++) {
        output = filter->b0 * stage_input + filter->state1[stage];
        filter->state1[stage] = filter->b1 * stage_input
                              - filter->a1 * output
                              + filter->state2[stage];
        filter->state2[stage] = filter->b2 * stage_input
                              - filter->a2 * output;
        stage_input = output;
    }

    return stage_input;
}
