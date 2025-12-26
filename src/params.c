/*
Central paramter storage. 
To save current values, sources.
Mostly for digital vs physical control.
*/

#include "params.h"

#define PARAM_MAX 256

static float g_params[PARAM_MAX];

void paramsInit(void) {
    for (int i = 0; i < PARAM_MAX; i++) g_params[i] = 0.0f;
}

bool paramsSet(uint16_t id, float value01) {
    if (id >= PARAM_MAX) return false;
    if (value01 < 0.0f) value01 = 0.0f;
    if (value01 > 1.0f) value01 = 1.0f;
    g_params[id] = value01;
    return true;
}

float paramsGet(uint16_t id) {
    if (id >= PARAM_MAX) return 0.0f;
    return g_params[id];
}
