#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int * intdup(int const * src, size_t len);

void setup_battery_check();

void setup_led();

int16_t get_battery_level();

uint8_t get_battery_percentage();

uint16_t get_battery_voltage_mv();
