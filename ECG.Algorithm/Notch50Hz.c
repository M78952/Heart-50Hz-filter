/*
 * MATLAB-equivalent 50 Hz notch filter for STM32F103.
 *
 * MATLAB source:
 *   Fs = 500;
 *   T = 1/Fs;
 *   Fc = 50;
 *   alpha = -2*cos(2*pi*Fc*T);
 *   beta = 0.96;
 *   b = [1, alpha, 1];
 *   a = [1, alpha*beta, beta^2];
 *   dataOut = dlsim(b, a, dataIn);
 *
 * With a[0] == 1, dlsim uses the following difference equation:
 *
 *   y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
 *          - a1*y[n-1] - a2*y[n-2]
 *
 * This implementation uses float instead of MATLAB's default double.  The
 * algorithm, coefficient signs, delay order, and zero initial state are the
 * same.  It is a single second-order section, not a cascaded fourth-order
 * filter.
 */

#include <math.h>
#include "Notch50Hz.h"

#ifndef ECG_NOTCH_PI
#define ECG_NOTCH_PI (3.14159265358979323846f)
#endif

static void ECG_Notch50Hz_ClearState(ECG_Notch50HzFilter *filter)
{
    filter->x1 = 0.0f;
    filter->x2 = 0.0f;
    filter->y1 = 0.0f;
    filter->y2 = 0.0f;
}

void ECG_Notch50Hz_Init(ECG_Notch50HzFilter *filter)
{
    float omega;
    float alpha;

    if (filter == (ECG_Notch50HzFilter *)0) {
        return;
    }

    /* Keep invalid settings from producing undefined coefficients. */
    if ((ECG_NOTCH_SAMPLE_RATE_HZ <= 0.0f) ||
        (ECG_NOTCH_CENTER_FREQ_HZ <= 0.0f) ||
        (ECG_NOTCH_CENTER_FREQ_HZ >=
         (0.5f * ECG_NOTCH_SAMPLE_RATE_HZ)) ||
        (ECG_NOTCH_BETA <= 0.0f) ||
        (ECG_NOTCH_BETA >= 1.0f)) {
        filter->b0 = 1.0f;
        filter->b1 = 0.0f;
        filter->b2 = 0.0f;
        filter->a1 = 0.0f;
        filter->a2 = 0.0f;
        ECG_Notch50Hz_ClearState(filter);
        return;
    }

    /* This is exactly 2*pi*Fc*T, with T=1/Fs. */
    omega = 2.0f * ECG_NOTCH_PI * ECG_NOTCH_CENTER_FREQ_HZ /
            ECG_NOTCH_SAMPLE_RATE_HZ;
    alpha = -2.0f * cosf(omega);

    /* b = [1, alpha, 1], a = [1, alpha*beta, beta^2]. */
    filter->b0 = 1.0f;
    filter->b1 = alpha;
    filter->b2 = 1.0f;
    filter->a1 = alpha * ECG_NOTCH_BETA;
    filter->a2 = ECG_NOTCH_BETA * ECG_NOTCH_BETA;

    /* MATLAB dlsim starts with zero delay states. */
    ECG_Notch50Hz_ClearState(filter);
}

void ECG_Notch50Hz_Reset(ECG_Notch50HzFilter *filter)
{
    if (filter == (ECG_Notch50HzFilter *)0) {
        return;
    }

    ECG_Notch50Hz_ClearState(filter);
}

float ECG_Notch50Hz_Process(ECG_Notch50HzFilter *filter, float input)
{
    float output;

    if (filter == (ECG_Notch50HzFilter *)0) {
        return input;
    }

    /* Direct-form-I implementation of the MATLAB dlsim recurrence. */
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
