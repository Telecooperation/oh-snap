#pragma once

#include "nvs_flash.h"
//SPIFFS
#include "FS.h"
#include "SPIFFS.h"

#include "mqtthandler.h"
#include "wifihandler.h"

//web
//#include "ESPAsyncWebServer.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

bool setup_filesys();
String readFile(fs::FS &fs, const char * path);
void listDir(fs::FS &fs, const char * dirname, mqttobjs &objs, bool iswebsub);
//uint16_t getDir(fs::FS &fs, const char * dirname, uint8_t levels, String[] filenames, uint16_t curfilecnt);
//void loadWebDir(const char * dirname, uint8_t levels, AsyncWebServer server);

mqttconfstruct read_mqtt_json(mqttconfstruct &mqttconf);
void write_mqtt_json(mqttconfstruct &mqttconf);

bool read_wifi_json(wificonf * mywificonf);
bool write_wifi_json(wificonf * mywificonf);
bool read_objconf_json(mqttobjstruct &mqttobj);

void moveFileOrFolder(const char * orig, const char * dest);
void write_temp_file(const char * filename, const size_t index, const char * data);
