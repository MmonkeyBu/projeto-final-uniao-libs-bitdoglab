#ifndef CALLBACKS_TIMER_H
#define CALLBACKS_TIMER_H

#include <stdbool.h>
#include "pico/stdlib.h"

void botao_callback(uint gpio, uint32_t eventos);
void botao_init(uint8_t pino);

#endif