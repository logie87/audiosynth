/*
User command parsing.
Set params, load/save patch, requests, etc.
*/

#include "command.h"
#include "../params.h"
#include <stdio.h>
#include <string.h>

static int parseSet(const char *line, unsigned *id, float *val) {
    // needs SET <id> <val>
    return sscanf(line, "SET %u %f", id, val) == 2;
}

void commandHandling(const char *line) {
    if (!line || !line[0]) return;

    if (strncmp(line, "PING", 4) == 0) {
        printf("PONG\n");
        return;
    }

    unsigned id = 0;
    float v = 0.0f;
    if (parseSet(line, &id, &v)) {
        if (paramsSet((uint16_t)id, v)) {
            printf("ACK SET %u %.4f\n", id, v);
        } else {
            printf("ERROR: Bad Parameter ID\n");
        }
        return;
    }

    printf("ERROR: Uknown CMD\n");
}
