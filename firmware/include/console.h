#pragma once

#include <stdint.h>

void console_callback_single(uint8_t tchannel, int32_t tval);
void console_callback_array(int32_t* values);

void console_task(void *pvParameter);
