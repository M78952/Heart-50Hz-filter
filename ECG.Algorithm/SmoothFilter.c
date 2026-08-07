/*
 * Causal moving-average smoother for the ECG stream.
 *
 * Difference equation for a full window of length M:
 *
 *     y[n] = (x[n] + x[n-1] + ... + x[n-M+1]) / M
 *
 * A running sum avoids adding all M samples every time.  During startup the
 * average is divided by the number of samples received so far, which avoids a
 * zero-filled startup transient.
 */

#include "SmoothFilter.h"

void ECG_SmoothFilter_Init(ECG_SmoothFilter *filter)
{
    uint8_t index;

    if (filter == (ECG_SmoothFilter *)0) {
        return;
    }

    filter->sum = 0.0f;
    filter->write_index = 0U;
    filter->sample_count = 0U;

    for (index = 0U; index < ECG_SMOOTH_WINDOW_SIZE; index++) {
        filter->samples[index] = 0.0f;
    }
}

float ECG_SmoothFilter_Process(ECG_SmoothFilter *filter, float input)
{
    float divisor;

    if (filter == (ECG_SmoothFilter *)0) {
        return input;
    }

    if (filter->sample_count < ECG_SMOOTH_WINDOW_SIZE) {
        filter->sample_count++;
    } else {
        filter->sum -= filter->samples[filter->write_index];
    }

    filter->samples[filter->write_index] = input;
    filter->sum += input;

    filter->write_index++;
    if (filter->write_index >= ECG_SMOOTH_WINDOW_SIZE) {
        filter->write_index = 0U;
    }

    divisor = (float)filter->sample_count;
    return filter->sum / divisor;
}
