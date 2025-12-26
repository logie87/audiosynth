/*
Outgoing packet production
param snapshots
meter values
scope frames and throttling rate
*/

#include "telemetry.h"
#include "../params.h"
#include <stdio.h>

void telemetryInit(void) {}

void telemetrySendFrame(void) {
    // placeholders for now!

    float cutoff = paramsGet(10);
    float res    = paramsGet(11);

    float env  = cutoff;                 // pretend envelope tracks cutoff
    float rms  = (cutoff + res) * 0.5f;  // pretend rms depends on both
    float peak = (rms * 0.8f) + 0.2f;    // pretend peak is higher than rms
    if (peak > 1.0f) peak = 1.0f;

    printf("TEL env=%.3f rms=%.3f peak=%.3f\n", env, rms, peak);
}
