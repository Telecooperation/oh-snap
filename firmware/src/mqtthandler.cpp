#include "mqtthandler.h"

#include <WiFi.h>

#include <PubSubClient.h>

#include "utilities.h"

#include "fshandler.h"

#include "defines.h"

/*

mqtt cmd line:
mosquitto
mosquitto_sub -t "esp32/touch"

//mqtt sonoff example
mosquitto_pub -t "cmnd/motti/power" -h 192.168.178.51 -m "toggle"


*/

boolean active = false;

void callback(char* topic, byte* message, uint16_t length);
void reconnect();

//const char* mqtt_server = "192.168.178.51";

#define MQTT_MAX_MSG_SIZE 50

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[MQTT_MAX_MSG_SIZE];
int32_t value = 0;

mqttconfstruct mqttconfig;

void setActiveObject(int16_t object){
  mqttconfig.activeObj = object;
  Serial.printf("obj %d is now active\r\n", mqttconfig.activeObj);
  if(object != -1){
    active = true;
  }
}

void setup_mqtt(mqttconfstruct &config, mqttobjs objs){
  mqttconfig = config;
  Serial.println(mqttconfig.srvip);
  client.setServer(mqttconfig.srvip, mqttconfig.srvport);
  client.setCallback(callback);
  Serial.println("__________MQTT SETUP COMPLETE___________");
}

char * mapMqttKeywords(uint8_t sensor, int32_t value, char * webmsg){
  Serial.println(String(sensor));
  Serial.println(String(value));
  Serial.println(webmsg);
  String msg = webmsg;
  msg.replace(MQTT_THRESH, String(mqttconfig.objs.at(mqttconfig.activeObj).sensors[sensor].threshold));
  msg.replace(MQTT_VALUE, String(value));
  msg.replace(MQTT_SENSOR, String(sensor));

  char * ret = (char *) malloc(sizeof(char) * (msg.length() + 1));
  msg.toCharArray(ret, msg.length() + 1);

  return ret;
}

long lastImpulse[SENSOR_COUNT] = {0};

void mqtt_callback_array(int32_t* values){
  if(active && (mqttconfig.activeObj != -1)){
    if (!client.connected()) {
      reconnect();
    }
    char str[11];
    String finalStr = "";

    for(uint8_t pos = 0; pos < SENSOR_COUNT; pos++){
      sprintf(str, "%2d: %5d ", pos, values[pos]);
      finalStr+=str;
    }
    //free(values);
    //Serial.println(finalStr);

    // TODO move this to regular task instead of callback
    client.loop();

    long now = millis();

    if (now - lastMsg > 1000) {
      lastMsg = now;
      char char_array[finalStr.length() + 1];
      strcpy(char_array, finalStr.c_str());
      client.publish("esp32/touch", char_array);
      int16_t actObj = mqttconfig.activeObj;
      mqttobjstruct activeObj = mqttconfig.objs.at(actObj);

      mqttsensorstruct curSensor;
      for(uint8_t pos = 0; pos < SENSOR_COUNT; pos++){

        curSensor = activeObj.sensors[pos];

        if(curSensor.isactive && now - lastImpulse[pos] > 2000){
          if(curSensor.greaterequal){
            //Serial.print("setting is greater equal");
            //Serial.println(curSensor.threshold);
            if(values[pos] >= curSensor.threshold){
              //Serial.println("value is greater equal");

              lastImpulse[pos] = now;
              char * mappedMsg = mapMqttKeywords(pos, values[pos], activeObj.sensors[pos].msg);
              client.publish(activeObj.sensors[pos].topic, mappedMsg);
              free(mappedMsg);
            }
          }else{
            if(values[pos] < curSensor.threshold){
              lastImpulse[pos] = now;
              //Serial.println("is smaller");
              //client.publish(activeObj.sensors[pos].topic, activeObj.sensors[pos].msg);
            }
          }

        }
      }


      if(values[1]>5000 && now - lastImpulse[1] > 2000){
        lastImpulse[1] = now;
      }

       String bat = "bat: " + String(get_battery_level()) + ", " + String(get_battery_voltage_mv()) + "mV (~" + String(get_battery_percentage()) + "%)";

       char bat_char_array[bat.length() + 1];
       //Serial.println(bat);
       strcpy(bat_char_array, bat.c_str());
       client.publish("esp32/battery", bat_char_array);
    }
  }
  free(values);
}

mqttobjstruct getMqttobj(){
  return mqttconfig.objs.at(mqttconfig.activeObj);
}

mqttobjs getMqttobjs(){
  return mqttconfig.objs;
}

uint16_t* getThresholdValues(){

  uint16_t* values;
  values = (uint16_t *) malloc(SENSOR_COUNT * sizeof(uint16_t));

  mqttobjstruct obj = mqttconfig.objs.at(mqttconfig.activeObj);
  for(uint8_t pos = 0; pos < SENSOR_COUNT; pos++){
    values[pos] = obj.sensors[pos].threshold;
    //Serial.println(String(values[pos]));
  }
  return values;
}

String getMqttMessagesString(){

    String values;
    //String messagesString = "5000,5001,5002,5003,5004,5005,5000,5001,5002,5003,5004,5005";
    // reserve space for all string pointers and the max size for each message
    //values = (String *) malloc(SENSOR_COUNT * (sizeof(String *) + MQTT_MAX_MSG_SIZE * sizeof(char)));
    /*for (int i = 0; i < SENSOR_COUNT; i++){
        values[i] = malloc((ID_LEN+1) * sizeof(char)); // yeah, I know sizeof(char) is 1, but to make it clear...
    }*/
    //values = (String *) malloc(SENSOR_COUNT * sizeof(String));

    mqttobjstruct obj = mqttconfig.objs.at(mqttconfig.activeObj);
    for(uint8_t pos = 0; pos < SENSOR_COUNT; pos++){
      values += String(obj.sensors[pos].msg) + ',';
      //Serial.println(String(values[pos]));
    }
    return values;
    //return messagesString;
}
//TODO change threshold to (u)int32
void changeThresholdValue(uint8_t sensor, uint16_t value){

  //mqttobjstruct obj = mqttconfig.objs.at(mqttconfig.activeObj);
  mqttconfig.objs.at(mqttconfig.activeObj).sensors[sensor].threshold = value;
  Serial.println("threshold changed");
  Serial.println(String(sensor));
  Serial.println(String(mqttconfig.objs.at(mqttconfig.activeObj).sensors[sensor].threshold));
  write_mqtt_json(mqttconfig);
}

void changeMqttMessage(uint8_t sensor, String message){
  message.toCharArray(mqttconfig.objs.at(mqttconfig.activeObj).sensors[sensor].msg, message.length() + 1);
  write_mqtt_json(mqttconfig);
}

void callback(char* topic, byte* message, uint16_t length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");

    }
    else if(messageTemp == "off"){
      Serial.println("off");

    }
  }
}

void reconnect() {
  // Loop 5 times or until we're reconnected
  uint8_t rec_trials = 5;
  while (rec_trials > 0 && !client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("OhSnap")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      const TickType_t xDelay = 5000 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
    }
    rec_trials--;
  }
  if(rec_trials == 0){
    Serial.println("reconnect failed.");
    active = false;
  }
}
