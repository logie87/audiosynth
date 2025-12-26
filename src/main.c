#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "params.h"
#include "./telemetry/command.h"
#include "./telemetry/telemetry.h"

#define TELEMETRY_HZ 30

static uint32_t micros_now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000000u + tv.tv_usec);
}

static int due(uint32_t now, uint32_t *next, uint32_t period) {
    if ((int32_t)(now - *next) >= 0) {
        *next += period;
        return 1;
    }
    return 0;
}

int main(void) {
    paramsInit();
    telemetryInit();

    const uint32_t tel_period = 1000000u / TELEMETRY_HZ;
    uint32_t next_tel = micros_now() + tel_period;
    char buffer[256];

    printf("READY\n");
    fflush(stdout);

    for (;;) {
        // input handler
        if (fgets(buffer, sizeof(buffer), stdin)) {
            size_t n = strlen(buffer);
            while (n && (buffer[n-1] == '\n' || buffer[n-1] == '\r')) buffer[--n] = '\0';
            commandHandling(buffer);
            fflush(stdout);
        }

        // periodic telemetry
        uint32_t now = micros_now();
        if (due(now, &next_tel, tel_period)) {
            telemetrySendFrame();
            fflush(stdout);
        }
    }
}
