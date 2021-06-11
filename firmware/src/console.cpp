#include "console.h"
#include <HardwareSerial.h>
#include "wifihandler.h"
#include "defines.h"
#include "fshandler.h"

#define OUTBUF_SIZE 150

char outbuf[OUTBUF_SIZE];

bool pause_output = false;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
const TickType_t xDelay = 100 / portTICK_PERIOD_MS;

void console_callback_single(uint8_t tchannel, int32_t tval){
  if(strlen(outbuf)==0){
    portENTER_CRITICAL(&mux);
    sprintf(outbuf, "%2d: %6d ", tchannel, tval);
    portEXIT_CRITICAL(&mux);
  }
}

void console_callback_array(int32_t* values){
  if(strlen(outbuf)==0){
    portENTER_CRITICAL(&mux);
    char str[12];
    for(int pos = 0; pos < SENSOR_COUNT; pos++){
      sprintf(str, "%2d: %6d ", pos, values[pos]);
      strcat(outbuf, str);
    }
    free(values);
    portEXIT_CRITICAL(&mux);
  }
}

void readFromConsole(){
  String str;

  if(Serial.available() > 0){
    str = Serial.readStringUntil('\r');
    if(String(WIFI_CONNECT_KEYWORD).equals(str)){
      pause_output = true;
      Serial.println("Please enter the Wi-Fi SSID:");
      Serial.read();
      while(!Serial.available()){
        vTaskDelay(xDelay);
      }
      String ssid = Serial.readStringUntil('\r');
      Serial.println(ssid);

      Serial.println("Now enter the Wi-Fi passphrase:");
      Serial.read();
      while(!Serial.available()){
        vTaskDelay(xDelay);
        // TODO wait for return. currently ignored
      }
      String pwd = Serial.readStringUntil('\r');
      Serial.println(pwd);
      wificonf mywificonf;
      char cssid[64];
      ssid.toCharArray(cssid, 64);
      char cpwd[64];
      pwd.toCharArray(cpwd, 64);
      mywificonf.ssid = cssid;
      mywificonf.pwd = cpwd;
      write_wifi_json(&mywificonf);
    }else{
      Serial.println("unknown command: " + str);
    }
    pause_output = false;
    //x = Serial.parseInt();
  }
}

void console_task(void *pvParameter) {
  while(1){
      vTaskDelay(CONSOLE_VAL_INTERVAL);
      readFromConsole();

      portENTER_CRITICAL(&mux);
      if(strlen(outbuf)!=0){
        if(!pause_output){
          Serial.println(outbuf);
        }
      }
      outbuf[0] = '\0';
      portEXIT_CRITICAL(&mux);
    }
}
