#ifndef __ECG_SMOOTH_FILTER_H
#define __ECG_SMOOTH_FILTER_H

#include <stdint.h>

/*
 * Moving-average smoothing after the 50 Hz notch.
 * Fs=500 Hz: a 5-point window spans 10 ms and adds about 4 ms delay.
 * Try 3U for less smoothing or 7U for stronger smoothing.
 */
#define ECG_SMOOTH_WINDOW_SIZE (5U)

typedef struct
{
    float samples[ECG_SMOOTH_WINDOW_SIZE];
    float sum;
    uint8_t write_index;
    uint8_t sample_count;
} ECG_SmoothFilter;

void ECG_SmoothFilter_Init(ECG_SmoothFilter *filter);

/* Process one sample after the notch filter. */
float ECG_SmoothFilter_Process(ECG_SmoothFilter *filter, float input);

#endif
