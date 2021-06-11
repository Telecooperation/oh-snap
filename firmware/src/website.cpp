#include "website.h"
#include "fshandler.h"
//#include <WebServer.h>
//WebServer server(80);
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);

#include "defines.h"
#include "utilities.h"

#define MAX_FSFNAME_LENGTH 32

#define WEB_THRESH "/readThresh"
#define WEB_TOUCH "/readTouch"
#define WEB_INFO "/readInfo"
#define WEB_MQTT_INFO "/readMqtt"
#define WEB_MQTT_MSG "/readMsg"


mqttobjs index_objs;


//void handleADC();

String touchValues = String(0) + ',' + String(2) + ',' + String(4) + ',' + String(6);

//char * objhtml prepareObjindex PROGMEM;

String generalInfo = "";

/*
void handleADC() {

  //String adcValue = String(t1) + ',' + String(t2) + ',' + String(t3) + ',' + String(t4);
  server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}*/

/*
void handleRoot();


void handleRoot() {
 //String s = MAIN_page; //Read HTML contents
 String websiteContent = readFile(SPIFFS, "/index.html");
 server.send(200, "text/html", websiteContent); //Send web page
}

//String adcValue = "0,0,5,0";
String adcValue = String(0) + ',' + String(2) + ',' + String(4) + ',' + String(6);

*/




String getInfo(){
    return String(get_battery_percentage());
}

String getMqttInfo(){
    return String(get_battery_percentage());
}



void touchFunc(char * path){
    server.on(path, HTTP_GET, [&](AsyncWebServerRequest *request){
        request->send(200, "text/plain", touchValues);
    });
}

void threshFunc(char * path){
    server.on(path, HTTP_GET, [&](AsyncWebServerRequest *request){

        uint16_t* values = getThresholdValues();

        String threshValues = "";
        for(uint8_t pos = 0; pos < SENSOR_COUNT; pos++){
          threshValues += String(values[pos]) + ',';
        }

        Serial.println(threshValues);
        free(values);
        request->send(200, "text/plain", threshValues);
    });
}

void mqttMsgFunc(char * path){
    server.on(path, HTTP_GET, [&](AsyncWebServerRequest *request){
        String messages = getMqttMessagesString();
        request->send(200, "text/plain", messages);
    });
}

void infoFunc(char * path){
    server.on(path, HTTP_GET, [&](AsyncWebServerRequest *request){
        String info = String(get_battery_percentage());;
        request->send(200, "text/plain", info);
    });
}

void serveValues(char projfolders[][MAX_FSFNAME_LENGTH], const uint8_t n, const char * srvTopic, void (*func)(char *)){

    for(uint8_t pos = 0; pos < n; pos++){
        char temp[32] = "";
        if(strlen(projfolders[pos]) != 0){
            temp[0] = '/';
            strcat(temp, projfolders[pos]);
        }
        strcat(temp, srvTopic);
        Serial.println(temp);
        (*func)(temp);
    }
}

void test(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  //Handle upload
  Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");Serial.println("FULKE IPLAOKD");

  //Serial.println(filename);
}

void handleParam(AsyncWebServerRequest *request) {
  AsyncWebParameter* p = request->getParam(request->params()-1);
  Serial.print(request->params());
  Serial.print(" Post ");
  Serial.println(p->name());
  request->send(200, "text/plain", "Hello World!");
}
//static void handle_update_progress_cb(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
static void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    //Handle upload
        Serial1.println("----UPLOAD-----");
        Serial1.print("FILENAME: ");
        Serial1.println(filename);
            Serial1.print("INDEX: ");
            Serial1.println(index);
        Serial1.print("LENGTH: ");
        Serial1.println(len);
        /*AsyncWebHeader *header = request->getHeader("X-File-Size");
        Serial1.print("File size: ");
        Serial1.println((size_t)header->value().toFloat());
        if (!Update.isRunning())
        {
            Serial1.print("Status Update.begin(): ");
            Serial1.println(Update.begin((size_t)header->value().toFloat()));
            Serial1.print("Update remaining: ");
            Serial1.println(Update.remaining());
        }
        else
        {
            Serial1.println("Status Update.begin(): RUNNING");
        }

    Serial1.print("FLASH BYTES: ");
    ESP.wdtDisable();
    Serial1.println(Update.write(data, len));
        Serial1.print("Update remaining: ");
    Serial1.println(Update.remaining());
    ESP.wdtEnable(10);
    Serial1.print("HEAP: ");
    Serial1.println(ESP.getFreeHeap());
    if (final)
    {
        Update.end();
        Serial1.print("----FINAL-----");
    }*/
}

static void upload_file(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    //Serial.println(request->argName(0));
    //request->arg((size_t) 1);
    //uid+="/" + filename;
    Serial.printf("index: %i\r\n", index);
    char filename_charray[FS_MAX_LENGTH];
    filename.toCharArray(filename_charray, FS_MAX_LENGTH);
    write_temp_file(filename_charray, index, (char *) data);
    Serial.println(filename);
}

void assemble_objindex(char * objhtml, mqttobjs objs){
    strcpy(objhtml, "<div class=\"w3-row-padding w3-theme-d3\">");
    char objhtmltemp[600];
    for(uint8_t pos = 0; pos < objs.size(); pos++){
        mqttobjstruct mqttobj = objs.at(pos);
        char * ident = mqttobj.ident;
        char * name = mqttobj.name;
        char * desc = mqttobj.desc;
        sprintf(objhtmltemp, "<div class=\"w3-third w3-section\">\
            <a href=\"%s/page.html\" target=\"_blank\">\
                <div class=\"w3-card-4 w3-light-grey\">\
              <img src=\"%s/idximg.png\" style=\"width:100%%\">\
                    <div class=\"w3-container w3-white\">\
                        <h4>%s</h4>\
                        <p>%s</p>\
                    </div>\
                </div>\
            </a>\
        </div>", ident,ident,name, desc);
        strcat(objhtml, objhtmltemp);
    }
    strcat(objhtml, "<div class=\"w3-third w3-section\">\
        <a href=\"addcustom/page.html\" target=\"_blank\">\
            <div class=\"w3-card-4 w3-light-grey\">\
          <img src=\"addcustom/idximg.png\" style=\"width:100%\">\
                <div class=\"w3-container w3-white\">\
                    <h4>Add custom object</h4>\
                    <p>Use the form to upload your custom object configuration and website.</p>\
                </div>\
            </div>\
        </a>\
    </div>\
    <div>");
}



void setup_website(mqttobjs objs){
    index_objs = objs;
    char projFolders[1][MAX_FSFNAME_LENGTH] = {""};//,"piano", "monitor", "game"};

    //make serving URLs available to all projects
    serveValues(projFolders, 1, WEB_TOUCH, touchFunc);
    serveValues(projFolders, 1, WEB_THRESH, threshFunc);
    serveValues(projFolders, 1, WEB_MQTT_MSG, mqttMsgFunc);
    serveValues(projFolders, 1, WEB_INFO, infoFunc);
    serveValues(projFolders, 1, WEB_MQTT_INFO, infoFunc);


    server.serveStatic("/", SPIFFS, "/web/").setDefaultFile("index.html");

  server.on("/chgThresh", HTTP_POST, [](AsyncWebServerRequest *request){
      Serial.println(request->argName(0));
      Serial.println(request->arg((size_t) 0));
      changeThresholdValue((uint8_t) (request->argName(0)).toInt(), (uint16_t) (request->arg((size_t) 0)).toInt());
      //request->send(200, "text/plain", "OK")
  });


  server.on("/setObject", HTTP_POST, [](AsyncWebServerRequest *request){
        const String arg = request->arg((size_t) 0);
        for(uint8_t pos = 0; pos < index_objs.size(); pos++){
            const String cur = String(index_objs.at(pos).ident);
            //if(strcmp(cur, arg) == 0){
            if(arg.equals(cur)){
                setActiveObject(pos);
            }
        }
        //changeMqttMessage((uint8_t) (request->argName(0)).toInt(), request->arg((size_t) 0));
        //changeMqttMsg((uint8_t) (request->argName(0)).toInt(), (uint16_t) (request->arg((size_t) 0)).toInt());
    });

  server.on("/chgMsg", HTTP_POST, [](AsyncWebServerRequest *request){
      Serial.println(request->argName(0));
      Serial.println(request->arg((size_t) 0));
      changeMqttMessage((uint8_t) (request->argName(0)).toInt(), request->arg((size_t) 0));
      //changeMqttMsg((uint8_t) (request->argName(0)).toInt(), (uint16_t) (request->arg((size_t) 0)).toInt());
  });

  server.on("/objindex", HTTP_GET, [](AsyncWebServerRequest *request){
//      mqttobjs objs = getMqttobjs();
      Serial.println(index_objs.size());
      char objhtml[600*(index_objs.size() + 1)] PROGMEM;
      assemble_objindex(objhtml, index_objs);
      request->send_P(200, "text/plain", objhtml);
  });

  server.on("/setWifiCred", HTTP_POST, [](AsyncWebServerRequest *request){
      wificonf mywificonf;
      String wifissid = request->argName(0);
      String wifipwd = request->arg((size_t) 0);
      char ssid[50];
      char pwd[50];
      wifissid.toCharArray(ssid, wifissid.length() + 1);
      mywificonf.ssid = ssid;
      wifipwd.toCharArray(pwd, wifipwd.length() + 1);
      mywificonf.pwd = pwd;
      write_wifi_json(&mywificonf);

      //changeMqttMessage((uint8_t) (request->argName(0)).toInt(), request->arg((size_t) 0));
      //changeMqttMsg((uint8_t) (request->argName(0)).toInt(), (uint16_t) (request->arg((size_t) 0)).toInt());

  });

  server.on("/chgMqttSettings", HTTP_POST, [](AsyncWebServerRequest *request){
      Serial.println(request->argName(0));
      Serial.println(request->arg((size_t) 0));
      //changeMqttSettings((uint8_t) (request->argName(0)).toInt(), request->arg((size_t) 0));
      //changeMqttMsg((uint8_t) (request->argName(0)).toInt(), (uint16_t) (request->arg((size_t) 0)).toInt());
  });


server.on("/finalize_customweb", HTTP_PUT, [](AsyncWebServerRequest *request){

    request->send(200, "text/plain", "Put route2");
});

  server.on("/put", HTTP_PUT, [](AsyncWebServerRequest *request){
      //Serial.println("OHHELLYEEEEEAAH");
      request->send(200, "text/plain", "Put route2");
      //request->send(200, "text/plain", touchValues);//To get update of ADC Value only
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
      request->send(200);
  }, handleUpload);

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
      Serial.println(request->argName(0));
      Serial.println(request->arg((size_t) 0));
      char filename[FS_MAX_LENGTH];
      (request->arg((size_t) 0)).toCharArray(filename, FS_MAX_LENGTH);

      char foldername[FS_MAX_LENGTH];
      (request->arg((size_t) 1)).toCharArray(foldername, FS_MAX_LENGTH);

      char oldpath[FS_MAX_LENGTH] = "/temp/";
      char newpath[FS_MAX_LENGTH] = "/web/";

      strcat(oldpath, filename);

      strcat(newpath, foldername);
      strcat(newpath, "/");
      strcat(newpath, filename);

      Serial.println("paths:");
      Serial.println("oldpath:");
      Serial.println(oldpath);
      Serial.println("newpath:");
      Serial.println(newpath);

      moveFileOrFolder(oldpath, newpath);


      //Serial.println(request->arg((size_t) 0));
      Serial.println(request->argName(1));
      Serial.println(request->arg((size_t) 1));
      Serial.println("receivedrequest");
      request->send(200);
  }, upload_file);

  // server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
  //     //handleParam(request);
  //              Serial.println("WWWWWWWWWWWWWOAKSDOFJEAOIFMOEAMF");},
  //
  //   ) {test(request, filename, index, data, len, final);}
  //   );
/*
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
      Serial.println("WWWWWWWWWWWWWOAKSDOFJEAOIFMOEAMF");
      int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);
            if (p->isFile())
            {
                Serial.printf("FILE[%s]: %s, size: %u \n", p->name().c_str(), p->value().c_str(), p->size());
            }
            else if (p->isPost())
            {
                Serial.printf("POST[%s]: %s \n", p->name().c_str(), p->value().c_str());
            }
        }
    request->send(200, "text/plain; charset=utf-8", "OK");
});

*/
/*
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
          int params = request->params();
          for (int i = 0; i < params; i++)
          {
              AsyncWebParameter *p = request->getParam(i);
              if (p->isFile())
              {
                  Serial.printf("FILE[%s]: %s, size: %u \n", p->name().c_str(), p->value().c_str(), p->size());
              }
              else if (p->isPost())
              {
                  Serial.printf("POST[%s]: %s \n", p->name().c_str(), p->value().c_str());
              }
          }
          request->send(200);
      },
                onUpload);
*/
  /*
  server.on("/piano/piano_3d.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/piano/piano_3d.png", "image/png");
  });

  server.on("/game/gamepad.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/game/gamepad.png", "image/png");
  });

  server.on("/addcustom/interface.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/addcustom/interface.png", "image/png");
  });

  server.on("/monitor/website_data.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/monitor/website_data.png", "image/png");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/favicon.ico", "image/x-icon");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/index.html", "text/html");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/index.html", "text/html");
  });

  /*server.on("/piano/piano.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/piano/piano.html", "text/html");
});*/


/*
  server.on("/piano/Tone.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/piano/Tone.min.js", "text/javascript");
  });


server.on("/simple.js", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(SPIFFS, "/web/piano/simple.js", "text/javascript");
  });

server.on("/simple.html", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(SPIFFS, "/web/simple.html", "text/html");
  });

  server.on("/monitor.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/monitor.html", "text/html");
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web/settings.html", "text/html");
  });



  server.on("/game/game.html", HTTP_GET, [](AsyncWebServerRequest *request){
      //todo parse complete fs
      //request->send(SPIFFS, "/web/game/game.html", "text/html");
  });


*/

/*server.onNotFound(onRequest);

server.onRequestBody(onBody);
*/
server.onFileUpload(test);
  server.begin();
}



/*
void website_task(void *pvParameter){
  const TickType_t xDelay = 25UL / portTICK_PERIOD_MS;
  String websiteContent = readFile(SPIFFS, "/index.html");
  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only

  server.begin();                  //Start server
  Serial.println("HTTP server started");


  while(1){
    server.handleClient();
    vTaskDelay(xDelay);
  }
}


void web_callback_single(uint8_t tchannel, uint16_t tval){
  //Serial.println(tchannel);
  //Serial.println(tval);
}*/


/*
void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isFile())
        {
            Serial.printf("onUpload FILE[%s]: %s, size: %u \n", p->name().c_str(), p->value().c_str(), p->size());
        }
        else if (p->isPost())
        {
            Serial.printf("onUpload POST[%s]: %s \n", p->name().c_str(), p->value().c_str());
        }
    }
    if (!index)
    {
        Serial.printf("UploadStart: %s\n", filename.c_str());
    }
    for (size_t i = 0; i < len; i++)
    {
        Serial.write(data[i]);
    }
    if (final)
    {
        Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
    }
}
*/
void web_callback_array(int32_t* values){
  touchValues = "";
  for(int pos = 0; pos < SENSOR_COUNT; pos++){
    touchValues += String(values[pos]) + ',';
  }
}
