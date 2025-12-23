#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "app.h"
#include "init.h"


// tick rates
#define CONTROL_HZ 1000u
#define TELEMETRY_HZ 60u // UI REFRESH

// scheduler
bool due(uint32_t now, uint32_t *next, uint32_t period) {
    if ((int32_t)(now - *next) >= 0) {
        *next += period;
        return true;
    } return false;
}

int main(void) {
    init_sys();
    app_init();
    const uint32_t controlPeriod = 1000000u / CONTROL_HZ;
    const uint32_t telemetryPeriod = 1000000u / TELEMETRY_HZ;
    uint32_t now = initMicro();
    uint32_t nextControl = now + controlPeriod;
    uint32_t nextTelemetry = now + telemetryPeriod;

    for(;;) {
        now = initMicro();

        // fast contorl tick
        // knobs or UI control -> params -> routing -> out
        if (due(now, &nextControl, controlPeriod)) {
            appControlTick();
        }
        // telemetry tick
        // send visualization snaps to ui, (or maybe stdout for now i think)
        if (due(now, &nextTelemetry, telemetryPeriod)) {
            appTelemetryTick();
        }
    }
}