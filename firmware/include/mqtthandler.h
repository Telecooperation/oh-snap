#pragma once

#include <stdint.h>

#include <vector>
#include <WString.h>

struct mqttsensorstruct;
struct mqttobjstruct;
struct mqttconfstruct;

typedef std::vector<mqttobjstruct> mqttobjs;

struct mqttsensorstruct
{
    bool isactive;
    char* topic;
    char* msg;
    bool greaterequal;
    uint16_t threshold;
    uint16_t mintriggertime;
    int infointerval;
};

struct mqttobjstruct
{
    char* name;
    char* ident;
    char* desc;
    mqttsensorstruct sensors[12];
};

struct mqttconfstruct
{
    char* srvip;
    int srvport;
    char* infotopic;
    int infointerval;
    int16_t activeObj;
    mqttobjs objs;
};


void mqtt_callback_array(int* values);
void setup_mqtt(mqttconfstruct &config, mqttobjs objs);
void mqttloop();

uint16_t* getThresholdValues();
String getMqttMessagesString();
void changeThresholdValue(uint8_t sensor, uint16_t value);
void changeMqttMessage(uint8_t sensor, String message);
void setActiveObject(int16_t object);
mqttobjstruct getMqttobj();
mqttobjs getMqttobjs();
