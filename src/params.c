/*
Central paramter storage. 
To save current values, sources.
Mostly for digital vs physical control.
*/

#include "params.h"

#define PARAM_MAX 256

static float g_params[PARAM_MAX];

typedef enum {
    PARAM_OK = 0,
    PARAM_ERR_BAD_ID,
    PARAM_ERR_BAD_VAL
} param_status_t;

typedef struct {
    uint16_t id;
    const char *name;
    float def;
    float min;
    float max;
} param_def_t;

static const param_def_t g_param_defs[] = {
    {2, "attack", 0.1f, 0.0f, 1.0f},
    {3, "decay", 0.5f, 0.0f, 1.0f},
    {4, "sustain", 0.7f, 0.0f, 1.0f},
    {5, "release", 0.3f, 0.0f, 1.0f},
    // mains    
    {10, "cutoff", 0.5f, 0.0f, 1.0f},
    {11, "resonance", 0.2f, 0.0f, 1.0f},
    {12, "vca_level", 0.0f, 0.0f, 1.0f},
    // too add to later
};

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
