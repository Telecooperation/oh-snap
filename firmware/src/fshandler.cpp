#include "fshandler.h"
#include <ArduinoJson.h>
#include "defines.h"



bool setup_filesys(){
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return false;
  }
  return true;
}


void moveFileOrFolder(const char * orig, const char * dest){
    if(SPIFFS.exists(dest)){
        SPIFFS.remove(dest);
    }
    Serial.println(String(SPIFFS.rename(orig,dest)));
}

void write_temp_file(const char * filename, const size_t index, const char * data) {
    char fullpath[FS_MAX_LENGTH];
    sprintf(fullpath, "/temp/%s", filename);
    //char fullpath[FS_MAX_LENGTH] = "custemp/file1.json";
    File file;
    if(index == 0){
        file = SPIFFS.open(fullpath, FILE_WRITE);
    }else{
        file = SPIFFS.open(fullpath, FILE_APPEND);
    }
    if(!file){
        Serial.println("There was an error opening the file for writing");
        return;
    }

    if(file.print(data)){
        Serial.println(fullpath);
        Serial.println("File was written");;
    } else {
        Serial.println("File write failed");
    }

    file.close();
}

void listDir(fs::FS &fs, const char * dirname, mqttobjs &objs, bool iswebsub){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(!file.isDirectory()){
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
            char * str = strdup(file.name());
            char * subfolder;
            subfolder = strtok (str, "/");
            if (subfolder!= NULL && strcmp("web",subfolder) == 0 ) {
                subfolder = strtok (NULL, "/");
                if(subfolder!=NULL){
                    char * file = NULL;
                    file = strtok (NULL, "/");
                    if (file != NULL){
                        //only add if required config.json is also found
                        if(strcmp(file, "config.json") == 0){
                            mqttobjstruct obj;
                            obj.ident = strdup(subfolder);
                            read_objconf_json(obj);
                            objs.push_back(obj);
                        }
                    }
                }
            }
        }
        file = root.openNextFile();
    }
}


bool read_wifi_json(wificonf * mywificonf){
    const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
    DynamicJsonDocument doc(capacity);

    const String wifiJson = readFile(SPIFFS,"/wificonf.json");
    DeserializationError error = deserializeJson(doc, wifiJson);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());

        // TODO do not init wifi with this file as there is a config problem.
    }else{
        mywificonf->ssid = strdup(doc["wifiname"]);
        mywificonf->pwd = strdup(doc["wifipwd"]);
        return true;
    }
    return false;
}

bool write_wifi_json(wificonf * mywificonf){

    const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
    DynamicJsonDocument doc(capacity);

    doc["wifiname"] = mywificonf->ssid;
    doc["wifipwd"] = mywificonf->pwd;

    serializeJson(doc, Serial);
    String wifiJson = "";
    serializeJson(doc, wifiJson);
    File file = SPIFFS.open("/wificonf.json", FILE_WRITE);
    if(!file){
         Serial.println("There was an error opening the file for writing");
    }
    if(file.print(wifiJson)) {
        Serial.println("File was written");
        file.close();
        return true;
    }else {
        Serial.println("File write failed");
    }
    file.close();
    return false;
}


//mqttconfstruct mqttconf;

mqttconfstruct read_mqtt_json(mqttconfstruct &mqttconf){
    StaticJsonDocument<3000> doc;
    const String mqttJson = readFile(SPIFFS,"/mainconf.json");

    //Serial.println(mqttJson);

    DeserializationError error = deserializeJson(doc, mqttJson);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());

        // TODO do not init mqtt as there is a config problem.
    }else{

    JsonObject mqttgeneral = doc["mqttgeneral"];
char* mqttgeneral_srvip = strdup(mqttgeneral["srvip"]); // "192.168.178.51"
int mqttgeneral_srvport = mqttgeneral["srvport"]; // 1883
char* mqttgeneral_infotopic = strdup(mqttgeneral["infotopic"]); // "ohsnap_info"
int mqttgeneral_infointerval = mqttgeneral["infointerval"]; // 10
int16_t mqttgeneral_activeObj = (int16_t) mqttgeneral["activeobj"]; // 0

mqttconf.srvip = mqttgeneral_srvip;
mqttconf.srvport = mqttgeneral_srvport;
mqttconf.infotopic = mqttgeneral_infotopic;
mqttconf.infointerval = mqttgeneral_infointerval;
mqttconf.activeObj = mqttgeneral_activeObj;

// JsonArray objs = doc["mqttobjects"];
//
//
//
// for(JsonObject obj : objs){
//     char* mqttobj_name = strdup(obj["name"]); // "pianoconf"
//     mqttobjstruct mqttobj;
//     mqttobj.name = mqttobj_name;
//
//     //TODO remove, make abstract for every obj
//     mqttobj.ident = "piano";
//     mqttobj.desc = "pianodesc";
//
//
//     //mqttsensorstruct sensors[12];
//     JsonArray sensors_array = obj["sensors"];
//     uint8_t sensor_cnt = 0;
//     for(JsonObject sensor : sensors_array){
//         mqttobj.sensors[sensor_cnt].isactive = sensor["isactive"]; // true
//         mqttobj.sensors[sensor_cnt].threshold = sensor["threshold"]; // 3000
//         mqttobj.sensors[sensor_cnt].mintriggertime = (uint16_t) sensor["mintriggertime"]; // 50
//         mqttobj.sensors[sensor_cnt].topic = strdup(sensor["topic"]); // "cmnd/motti/power"
//         mqttobj.sensors[sensor_cnt].msg = strdup(sensor["msg"]); // "toggle"
//         sensor_cnt++;
//     }
//     //mqttobj.sensors = sensors;
//     mqttconf.objs.push_back(mqttobj);
//}
/*
JsonObject mqttobjects_mqtt_pianoconf = doc["mqttobjects"]["obj0"];
char* mqttobjects_mqtt_pianoconf_name = strdup(mqttobjects_mqtt_pianoconf["name"]); // "pianoconf"
//bool mqttobjects_mqtt_pianoconf_gtlt = mqttobjects_mqtt_pianoconf["defgteq"]; // true
//uint16_t mqttobjects_mqtt_pianoconf_threshold = (uint16_t) mqttobjects_mqtt_pianoconf["defthreshold"]; // 10000
//uint16_t mqttobjects_mqtt_pianoconf_mintriggertime = (uint16_t) mqttobjects_mqtt_pianoconf["defmintriggertime"]; // 50
//bool mqttobjects_mqtt_pianoconf_retrigger = mqttobjects_mqtt_pianoconf["defretrigger"]; // false
//const char* mqttobjects_mqtt_pianoconf_cmd = strdup(mqttobjects_mqtt_pianoconf["defcmd"]); // "hello there!"

mqttobjstruct mqttobj;

for(int pos = 0; pos < 12; pos++){

}

JsonObject mqttobjects_mqtt_pianoconf_sensors_0 = mqttobjects_mqtt_pianoconf["sensors"]["0"];
bool mqttobjects_mqtt_pianoconf_sensors_0_isactive = mqttobjects_mqtt_pianoconf_sensors_0["isactive"]; // true
int mqttobjects_mqtt_pianoconf_sensors_0_threshold = mqttobjects_mqtt_pianoconf_sensors_0["threshold"]; // 3000
int mqttobjects_mqtt_pianoconf_sensors_0_mintriggertime = (uint16_t) mqttobjects_mqtt_pianoconf_sensors_0["mintriggertime"]; // 50
char* mqttobjects_mqtt_pianoconf_sensors_0_topic = strdup(mqttobjects_mqtt_pianoconf_sensors_0["topic"]); // "cmnd/motti/power"
char* mqttobjects_mqtt_pianoconf_sensors_0_msg = strdup(mqttobjects_mqtt_pianoconf_sensors_0["msg"]); // "toggle"

//mqttsensorstruct sensors[12];
//mqttconf.mqttobjs[0];


/*
mqttobj.name = mqttobjects_mqtt_pianoconf_name;
mqttobj.sensors[0].isactive = mqttobjects_mqtt_pianoconf_sensors_0_isactive;
mqttobj.sensors[0].threshold = mqttobjects_mqtt_pianoconf_sensors_0_threshold;
mqttobj.sensors[0].mintriggertime = mqttobjects_mqtt_pianoconf_sensors_0_mintriggertime;
mqttobj.sensors[0].topic = mqttobjects_mqtt_pianoconf_sensors_0_topic;
mqttobj.sensors[0].msg = mqttobjects_mqtt_pianoconf_sensors_0_msg;

mqttconf.objs.push_back(mqttobj);
*/
//obj0.sensors = sensors;



//Serial.println(mqttgeneral_infotopic);
}
return mqttconf;
}


void write_mqtt_json(mqttconfstruct &mqttconf){
    const size_t capacity = 3000;//JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(12) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) + 12*JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7);
DynamicJsonDocument doc(capacity);

JsonObject mqttgeneral = doc.createNestedObject("mqttgeneral");
mqttgeneral["srvip"] = mqttconf.srvip;
mqttgeneral["srvport"] = mqttconf.srvport;
mqttgeneral["infotopic"] = mqttconf.infotopic;
mqttgeneral["infointerval"] = mqttconf.infointerval;
mqttgeneral["activeobj"] = mqttconf.activeObj;

JsonArray mqttobjects = doc.createNestedArray("mqttobjects");

for(mqttobjstruct mqttobj : mqttconf.objs){
    JsonObject mqttobject = mqttobjects.createNestedObject();
    mqttobject["name"] = mqttobj.name;
    //mqttobject["defgteq"] = mqttobj.defgteq;
    //mqttobject["defthreshold"] = mqttobj.defthreshold;
    //mqttobject["defmintriggertime"] = mqttobj.defmintriggertime;
    //mqttobject["defretrigger"] = mqttobj.defretrigger;
    //mqttobject["defcmd"] = mqttobj.defcmd;

    JsonArray mqttobjects_0_sensors = mqttobject.createNestedArray("sensors");
    for(uint8_t pos = 0; pos < 12; pos++){
        JsonObject mqttobjects_0_sensors_0 = mqttobjects_0_sensors.createNestedObject();
        mqttobjects_0_sensors_0["isactive"] = mqttobj.sensors[pos].isactive;
        mqttobjects_0_sensors_0["gteq"] = mqttobj.sensors[pos].greaterequal;
        mqttobjects_0_sensors_0["threshold"] = mqttobj.sensors[pos].threshold;
        mqttobjects_0_sensors_0["mintriggertime"] = mqttobj.sensors[pos].mintriggertime;
        mqttobjects_0_sensors_0["topic"] = mqttobj.sensors[pos].topic;
        mqttobjects_0_sensors_0["msg"] = mqttobj.sensors[pos].msg;
    }

}
String mqttJson = "";
serializeJson(doc, mqttJson);
File file = SPIFFS.open("/mqttconf.json", FILE_WRITE);
if(!file){
     Serial.println("There was an error opening the file for writing");
     return;
}
if(file.print(mqttJson)) {
    Serial.println("File was written");
}else {
    Serial.println("File write failed");
}
file.close();
}


String readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return "";
    }

    Serial.println("- read from file:");
    String fileContent = "";
    if(file.available()){
      fileContent = (file.readString());
    }

    return fileContent;
}

bool read_objconf_json(mqttobjstruct &obj){
    const size_t capacity = JSON_ARRAY_SIZE(12) + 12*JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(8) + 2000;
    DynamicJsonDocument doc(capacity);
    char * ident = obj.ident;
    char path[50] = "/web/";
    strcat(path, ident);
    strcat(path, "/config.json");
    Serial.println(path);
    File file = SPIFFS.open(path);
    Serial.print("File size: ");
    Serial.println(file.size());
    file.close();

    const String objconf_json = readFile(SPIFFS,path);


    DeserializationError error = deserializeJson(doc, objconf_json);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
    }else{
        char * name = strdup(doc["name"]);
        char * desc = strdup(doc["description"]);
        obj.name = name;
        obj.desc = desc;
        bool defgteq = doc["defgteq"]; // true
        int defthreshold = doc["defthreshold"]; // 10000
        int defmintriggertime = doc["defmintriggertime"]; // 50
        bool defretrigger = doc["defretrigger"]; // false
        const char* defcmd = doc["defcmd"]; // "hello there!"



        JsonArray sensors_array = doc["sensors"];
        uint8_t sensor_cnt = 0;
        for(JsonObject sensor : sensors_array){
            if(sensor_cnt<SENSOR_COUNT){
                obj.sensors[sensor_cnt].isactive = sensor["isactive"]; // true
                obj.sensors[sensor_cnt].greaterequal = sensor["gteq"]; // true
                obj.sensors[sensor_cnt].threshold = sensor["threshold"]; // 3000
                obj.sensors[sensor_cnt].mintriggertime = (uint16_t) sensor["mintriggertime"]; // 50
                obj.sensors[sensor_cnt].topic = strdup(sensor["topic"]); // "cmnd/motti/power"
                obj.sensors[sensor_cnt].msg = strdup(sensor["msg"]); // "toggle"
            }
            sensor_cnt++;
        }
    }
    return true;
}
