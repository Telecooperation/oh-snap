#pragma once

typedef void (*cb_touch_single)(uint8_t tchannel, int32_t tval);
typedef void (*cb_touch_array)(int32_t* tvals);


void toucheck_task(void *pvParameter);
void register_web_callback(cb_touch_array callback_func);
void register_console_callback(cb_touch_array callback_func);
void register_mqtt_callback(cb_touch_array callback_func);
