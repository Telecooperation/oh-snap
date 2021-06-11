
/*
 Arduino connections
 Arduino   FDC1004 board
-------   -------------
     5V - Vin
    GND - GND
     A4 - SDA
     A5 - SCL
*/
#include <Wire.h>
#include "FDC1004.h"
#include <stdio.h>

#include "utilities.h"

#include "fdctouch.h"

#include "defines.h"

void selectI2C(uint8_t i);
int measureChannel(uint8_t nr);

//i2c ports on the TCA9546A where the FDC chips are connected
int i2cPorts[FDC_CHIPS] = {0,1,2};

int32_t values[SENSOR_COUNT];
//low-pass filter for sensor values
float alpha = 0.1;


#define MAX_MEAS_VALUE 112865

#define UPPER_BOUND  0X4000                 // max readout capacitance
#define LOWER_BOUND  (-1 * UPPER_BOUND)

#define IDLE_VALUES 10

int32_t capdac[SENSOR_COUNT] = {0};

#define MEASUREMENT_TYPE 0

int32_t idleValueCollector[SENSOR_COUNT][IDLE_VALUES];
uint8_t idle_range = 10;
uint8_t idle_meas = 0;

int32_t idleValues[SENSOR_COUNT] = {0};
int16_t measurementscount = 0;

int16_t relativeTrigger = 500;
char result[100];


FDC1004 FDC;

// first values are mostly invalid, ignore these
uint16_t dismissMeas = DISMISS_FIRST_N_MEASUREMENTS;

int32_t lowpassfilter(uint8_t cur_sensor, int32_t capval);
void adjust_capdac(uint8_t cur_sensor, int16_t msb);

// callback functions
void (*led_cb)(uint8_t tchannel, int32_t tval);
void (*web_cb_single)(uint8_t tchannel, int32_t tval);
void (*web_cb_array)(int32_t* values);
void (*console_cb_single)(uint8_t tchannel, int32_t tval);
void (*console_cb_array)(int32_t* values);
void (*mqtt_cb_array)(int32_t* values);


//init callback functions
void init_cb(){
  led_cb = NULL;
  web_cb_single = NULL;
  web_cb_array = NULL;
  console_cb_single = NULL;
  console_cb_array = NULL;
  mqtt_cb_array = NULL;
}

void led_callback(uint8_t tchannel, int32_t tval){
  ledcWrite(1, 255);
  //Serial.println(tchannel);
  //Serial.println(tval);
}

void register_led_callback(cb_touch_single callback_func){
  led_cb = callback_func;
}

void unregister_led_callback(){
  led_cb = NULL;
}

void register_web_callback(cb_touch_array callback_func){
  web_cb_array = callback_func;
}

void unregister_web_callback(){
  web_cb_array = NULL;
}

void register_console_callback(cb_touch_array callback_func){
  console_cb_array = callback_func;
}

void unregister_console_callback(){
  console_cb_array = NULL;
}

void register_mqtt_callback(cb_touch_array callback_func){
  mqtt_cb_array = callback_func;
}

void unregister_mqtt_callback(){
  mqtt_cb_array = NULL;
}

void toucheck_task(void *pvParameter) {
  Wire.begin(); //i2c begin

  init_cb();

  register_led_callback(led_callback);

  while(1){
      uint8_t cur_sensor = 0;

			// for each FDC1004, check every channel
			for(uint8_t channel = 0; channel < INPUTS_PER_FDC; channel++){

			// trigger channel on every single FDC1004
				for(uint8_t icpos = 0; icpos < FDC_CHIPS; icpos++){
						selectI2C(icpos);
						cur_sensor = (icpos * (FDC_CHIPS + 1)) + channel;
						FDC.configureMeasurementSingle(MEASUREMENT_TYPE, channel, capdac[cur_sensor]);
						FDC.triggerSingleMeasurement(MEASUREMENT_TYPE, FDC1004_100HZ);

					}
					delay(18); // wait 20 milliseconds

      // check every single FDC1004
      for(uint8_t icpos = 0; icpos < FDC_CHIPS; icpos++){
          selectI2C(icpos);
					cur_sensor = (icpos * (FDC_CHIPS + 1)) + channel;
              //increase current sensor index

  		        //wait for completion

  		        uint16_t value[2];
  		        if (! FDC.readMeasurement(MEASUREMENT_TYPE, value))
  		          {
                  // calculate actual capacity
  		              int16_t msb = (int16_t) value[0]; // raw value
  				          //FDC.calibrateGain((uint16_t) msb);
  		    int32_t capacitance = ((int32_t)ATTOFARADS_UPPER_WORD) * ((int32_t)msb); //in attofarads
  		    capacitance /= 1000;   //in femtofarads
  		    capacitance += ((int32_t)FEMTOFARADS_CAPDAC) * ((int32_t)capdac[cur_sensor]);

          int32_t filtered_val = lowpassfilter(cur_sensor, capacitance);


				//Serial.printf("ic%d[ch%d]: %6d fF ", icpos, channel, values[cur_sensor]);

					adjust_capdac(cur_sensor, msb);
  /*
  				if(measurementscount == DISMISS_N_MEASUREMENTS){
  					idleValues[cur_sensor] = capacitance;
  					Serial.printf("idle value reg: %d", idleValues[cur_sensor]);
  				}
  				if(measurementscount == -1){
  					if(capacitance - relativeTrigger > idleValues[cur_sensor]){
  						//Serial.printf("OAENUF%d\r\n", idleValues[cur_sensor]);
  					}
  				}
  */
	/*
  				if(capacitance < MAX_MEAS_VALUE / 2){
  					idleValueCollector[cur_sensor][idle_meas] = capacitance;

  				}
*/
  	  	}
  		}
  	}

/*
  	if(measurementscount == DISMISS_FIRST_N_MEASUREMENTS){
  		measurementscount = -1;
  	}

  	if(measurementscount >= 0 && measurementscount<DISMISS_FIRST_N_MEASUREMENTS){
  		//wait until sensor values settle down
  		measurementscount++;
  	}
  	if(idle_meas == IDLE_VALUES){
  		idle_meas = 0;
  		for(uint8_t channelpos = 0; channelpos < 4; channelpos++){
  			idleValues[channelpos] = 0;
  			for(uint8_t valpos = 0; valpos < IDLE_VALUES; valpos++){
  				idleValues[channelpos]+=idleValueCollector[channelpos][valpos];
  			}
  			idleValues[channelpos] /= IDLE_VALUES;
  		}
  		//idleValues
  	//}
  	}else{
  		idle_meas++;
  	}
*/




/*

    if(dismissMeas > 0){
        dismissMeas--;
    }

    for (int i=0; i<SENSOR_COUNT; i++) {

      // FDC1004s are processed consecuticely
      selectI2C(i2cPorts[i/INPUTS_PER_FDC]);

      // measure specific channel on the current FDC1004
      int value = measureChannel(i%4);

      //apply low-pass filter
      if (values[i] == 0) {
        //just take first value instead of starting filter from 0
        values[i] = value;
      } else {
        values[i] = (values[i] * (1 - alpha)) + ((value - values[i]) * alpha);
      }

      if (dismissMeas <= 0) {

        if(values[0] > 5000){
          led_cb(i,values[i]);

        }else{
          ledcWrite(1, 0);
          //web_cb(i,values[i]);
        }
      }
    }

    if(web_cb_array!=NULL){
      web_cb_array(values);
    }
    if(console_cb_array!=NULL){
      console_cb_array(intdup(values,SENSOR_COUNT));
    }
    if(mqtt_cb_array!=NULL){
      mqtt_cb_array(intdup(values,SENSOR_COUNT));
    }

  }*/
//Serial.println();


		if(web_cb_array!=NULL){
			web_cb_array(values);
		}

		for(uint8_t pos = 0; pos < FDC_CHIPS * 4; pos++){
			Serial.printf("%6d;", values[pos]);
		}
		Serial.println();
		/*if(console_cb_array!=NULL){
			console_cb_array(intdup(values,SENSOR_COUNT));
		}*/

		if(mqtt_cb_array!=NULL){
      mqtt_cb_array(intdup(values,SENSOR_COUNT));
    }

	}
}


//measure a single channel
int32_t measureChannel(uint8_t nr) {
  FDC.configureMeasurementSingle(0, nr, 0);
  FDC.triggerSingleMeasurement(0, MEASUREMENT_FREQUENCY);

  //wait for completion, depending on frequency
  switch (MEASUREMENT_FREQUENCY) {
    case FDC1004_100HZ:
      vTaskDelay((1000.0/100 + 1)/portTICK_PERIOD_MS);
      break;
    case FDC1004_200HZ:
      vTaskDelay((1000.0/200 + 1)/portTICK_PERIOD_MS);
      break;
    case FDC1004_400HZ:
      vTaskDelay((1000.0/400 + 1)/portTICK_PERIOD_MS);
      break;
  }

  uint16_t value[2];
  if (! FDC.readMeasurement(0, value)) {
    return (int32_t) value[0];
  }
  return 0;
}

int32_t lowpassfilter(uint8_t cur_sensor, int32_t capval){
        if (values[cur_sensor] == 0) {
          //just take first value instead of starting filter from 0
          values[cur_sensor] = capval;
        } else {
          values[cur_sensor] = (values[cur_sensor] * (1 - alpha)) + ((capval - values[cur_sensor]) * alpha);
        }
        return values[cur_sensor];
}

// adjust capdac accordingly
void adjust_capdac(uint8_t cur_sensor, int16_t msb){
		if (msb > UPPER_BOUND){
			if (capdac[cur_sensor] < FDC1004_CAPDAC_MAX)
			capdac[cur_sensor]++;
		}	else if (msb < LOWER_BOUND){
			if (capdac[cur_sensor] > 0){
				capdac[cur_sensor]--;
			}
		}
}

//select an I2C port on the TCA9546A I2C multiplexer
void selectI2C(uint8_t i) {
  if (i > 3) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}
