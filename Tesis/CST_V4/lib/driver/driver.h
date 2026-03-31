#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    float frequency;
    float amplitude;
    bool running;
} vibration_config_t;

void vibration_init();
void vibration_start();
void vibration_stop();
void vibration_set_frequency(float f);
void vibration_set_amplitude(float a);

void vibration_update(void);

#endif