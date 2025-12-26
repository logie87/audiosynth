#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>
#include <stdbool.h>

void paramsInit(void);
bool paramsSet(uint16_t id, float value01);
float paramsGet(uint16_t id);

#endif
