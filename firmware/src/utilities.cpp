#include "utilities.h"
#include <driver/adc.h>
#include <esp_system.h>
#include <Arduino.h>
#include "defines.h"

int * intdup(int const * src, size_t len)
{
   int * p = (int*) malloc(len * sizeof(int));
   memcpy(p, src, len * sizeof(int));
   return p;
}

void setup_battery_check(){
   adc1_config_width(ADC_WIDTH_BIT_12);
   adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);
}

int16_t get_battery_level(){
   return (int16_t) adc1_get_raw(ADC1_CHANNEL_7);
}

uint8_t get_battery_percentage(){
   int16_t value = adc1_get_raw(ADC1_CHANNEL_7) - 2150;
   if(value < 0){
      return 0;
   } else if(value > 280){
      return 100;
   }
   return (uint8_t) ((value * 10) / 28);
}



uint16_t get_battery_voltage_mv(){
   // 2480 4.13V (USB)
   // 2470 4.06V (USB)
   // 2448 4.02V (USB)
   // 2448 4.12V (ohne USB, direkt nach laden)
   // 2373 4.00V (direkt nach USB entfernen, nicht voll geladen)

   uint16_t value = (adc1_get_raw(ADC1_CHANNEL_7) * VOLTAGE_CONV_FACTOR);//* 6.9) / 4096;

   return (uint16_t) value;
}

void led_action(int8_t type, int8_t duration){
   //type 0: off
   //positive except  0: max brightness
   //negative values: pulsation


}


void setup_led(){
  // led indicator setup
  ledcAttachPin(LED_PIN, LED_CHANNEL);
  ledcSetup(LED_CHANNEL, 12000, 8);   // 12 kHz PWM, 8 bit resolution
  //ledcWrite(LED_CHANNEL, 255);        // Write a test value of 255 to channel 1
}
