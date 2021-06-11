#include <stdio.h>
#include <esp_system.h>
#include <FreeRTOS.h>
#include "fshandler.h"
#include "wifihandler.h"
#include "console.h"
#include "mqtthandler.h"
#include "utilities.h"
#include "defines.h"
// bt handling // currently disabled due to high progmem use
//#include "bthandler.h"
#include "website.h"
#include "fdctouch.h"

bool wifi_connected = false;

mqttconfstruct mqttconf;
mqttobjs objs;

void setup() {
  Serial.begin(115200);

  setup_led();
  //setup_bluetooth();

  if(setup_filesys()){

    mqttobjs objs;
    listDir(SPIFFS, "/", objs, false);
    mqttconf = read_mqtt_json(mqttconf);
    mqttconf.objs = objs;
    for(mqttobjstruct obj : objs){
      read_objconf_json(obj);
    }

    setup_battery_check();

    wifi_connected = wifi_connect();

    xTaskCreate(toucheck_task, "toucheck_task", 4096, NULL, 4, NULL);
    xTaskCreate(console_task, "console_task", 4096, NULL, 5, NULL);

    if(!wifi_connected){
      Serial.println("No Wi-Fi connection available, MQTT and web services won't work.");
    }else{
      setup_website(objs);
      //xTaskCreate(website_task, "website_task", 2048, NULL, 5, NULL);
      register_web_callback(web_callback_array);
      register_mqtt_callback(mqtt_callback_array);

      setup_mqtt(mqttconf, objs);
    }
    delay(100); // console cb reg had problems with immediate call
    register_console_callback(console_callback_array);

  /*
    ledcAttachPin(25, 2);    //Configure variable led, pin 18 to channel 1
    ledcSetup(2, 12000, 8);   // 12 kHz PWM and 8 bit resolution
    ledcWrite(2, 255);        // Write a test value of 100 to channel 1
  */
  }
}


void loop(){
  // tasks outsourced to FreeRTOS tasks
  //Serial.println(get_battery_level());


}
