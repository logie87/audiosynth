#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>

#include "params.h"
#include "./telemetry/command.h"
#include "./telemetry/telemetry.h"

#define TELEMETRY_HZ 30
// ui rate telemetry emissions/second

// get current time in microseconds
static uint32_t microsNow(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000000u + tv.tv_usec);
} // this will wrap around approx every 71.58 minutes,
// 2^32 -> 4294.967296 / 60 = 71.58.

/* is current time at or past next.
if yes, update *next to next scheduled time, return 1 meaning run task now
if not, return 0.
*/
static int due(uint32_t now, uint32_t *next, uint32_t period) {
    if ((int32_t)(now - *next) >= 0) {
        *next += period;
        return 1;
    }
    return 0;
}


// check for data available on stdin that won't block runtime.
static int stdinAvailable(void) {
    fd_set readfds; // set of file descriptors to read
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval tv;
    tv.tv_usec = 0;
    tv.tv_sec = 0;
    int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    return (ret > 0) && FD_ISSET(STDIN_FILENO, &readfds);
    // returns when at least one FD is ready for reading, or timeout happens.
}

int main(void) {
    paramsInit();
    telemetryInit();

    const uint32_t telPeriod = 1000000u / TELEMETRY_HZ; // not perfect 30hz but itll do i suppose
    uint32_t nextTel = microsNow() + telPeriod;
    char buffer[256];

    printf("READY\n");
    printf("HELLO protocol=1.0, name=synthsim\n");
    fflush(stdout);

    for (;;) {
        // periodic telemetry. should be on time no matter stdin activity
        uint32_t now = microsNow();
        if (due(now, &nextTel, telPeriod)) {
            telemetrySendFrame();
            fflush(stdout);
        }

        // needed to check for data to ensure telemetry is never halted by stdin waiting for input.
        if (stdinAvailable()) { // if true, has some data ready to read.
            // read command from stdin
            if (!fgets(buffer, sizeof(buffer), stdin)) {
                break;
            }
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                size_t n = strlen(buffer);
                while (n && (buffer[n - 1] == '\n' || buffer[n - 1] == '\r')) {
                    buffer[--n] = '\0';
                }
                commandProcess(buffer);
                fflush(stdout);
            }
        }
        usleep(1000); // 1ms sleep
    }
}
