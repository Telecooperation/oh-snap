#pragma once

//mqtt keywords
#define MQTT_THRESH "<threshold>"
#define MQTT_VALUE "<value>"
#define MQTT_SENSOR "<sensor>"

//sensors
#define TCAADDR 0x70 // for v1.0 try 0x70-0x73

#define FDC_CHIPS 3													//how many FDC1004 chips are on the sensor board
#define INPUTS_PER_FDC 4										//how many inputs are used per FDC1004 chip
#define DISMISS_FIRST_N_MEASUREMENTS 50			//how many values to dismiss to give sensor time to calibrate
#define MEASUREMENT_FREQUENCY FDC1004_100HZ //the frequency of a single reading. slower means higher precision. can be 100HZ, 200HZ, 400HZ.

#define SENSOR_COUNT FDC_CHIPS*INPUTS_PER_FDC

//battery
#define VOLTAGE_CONV_FACTOR 2*3.45*1000/4096

//wifi
#define WIFI_CONNECT_KEYWORD "wificonnect"

#define LED_PIN 4
#define LED_CHANNEL 1

#define FS_MAX_LENGTH 32


#define CONSOLE_VAL_INTERVAL 100
