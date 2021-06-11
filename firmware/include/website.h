#pragma once

#include <stdint.h>
#include "mqtthandler.h"

void website_task(void *pvParameter);

void web_callback_single(uint8_t tchannel, uint16_t tval);
void web_callback_array(int* values);
void setup_website(mqttobjs objs);
void assemble_objindex(char * objhtml, mqttobjs objs);
