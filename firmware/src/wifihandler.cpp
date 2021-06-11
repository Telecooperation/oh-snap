#include "wifihandler.h"
#include "fshandler.h"
#include "defines.h"
WiFiMulti wifiMulti;

//const char * ssid = "honeypot";
//const char * wifipwd = "rfmusebrot";

bool wifi_connect(){
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

  //https://github.com/esp8266/Arduino/issues/2235
  WiFi.disconnect();


  Serial.println("WIFI connect");
  //ESP32 connects to your wifi -----------------------------------
    //WiFi.hostname(WiFi_hostname);
    WiFi.mode(WIFI_STA); //Connectto your wifi

    //WiFi.begin(ssid, wifipwd);

    wificonf mywificonf;
    mywificonf.ssid = "change\0";

    bool wificred = read_wifi_json(&mywificonf);

    //Serial.println(wifiMulti.APlist.at((size_t) 0));
    if(wificred && *(mywificonf.ssid) != 0){
      /*Serial.println("conf ssid != 0");
      char x = (char)(*(mywificonf.ssid));
      char buffer[10];
      sprintf(buffer,"number: %d",x);
      Serial.println(buffer);*/
      wifiMulti.addAP(mywificonf.ssid, mywificonf.pwd);
      //todo: after successful connection, clear wificonf.json

      Serial.println("Connecting to ");
      Serial.print(mywificonf.ssid);
      //Wait for WiFi to connect
      uint8_t count = 0;

      if(wifiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());


        ledcWrite(LED_CHANNEL, 255);
        /*if(wificred){
          mywificonf.ssid = "";
          mywificonf.pwd = "";
          write_wifi_json(&mywificonf);
        }*/
        return true;
      }

      while (count++ < 10){
        if(wifiMulti.run() != WL_CONNECTED) {
          Serial.println("WiFi not connected!");
          vTaskDelay(xDelay);
        }
      }
    }
    // could not establish a wifi connection, so start ap mode...
    Serial.println("No Wi-Fi set up yet. Starting AP mode...");
    //todo ap mode
    WiFi.mode(WIFI_AP);

    const char* assid = "OhSnapAP";
    const char* asecret = "12345678";

    //access point part
    Serial.println("Creating Accesspoint");
    IPAddress local(192,168,1,1);
    IPAddress gateway(127,0,0,1);
    IPAddress subnet(255,255,255,0);
    WiFi.softAPConfig(local, gateway, subnet);
    WiFi.softAP(assid,asecret);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());


    //Wifi.softAP(, optional const char *passphrase = NULL, optional int channel = 1, optional int ssid_hidden = 0, optional int max_connection = 4)

    return true;

/*    while(WiFi.waitForConnectResult() != WL_CONNECTED){
        Serial.print(".");
        delayMicroseconds(100000);
        if(count++ > 20){

        }
      }

    //If connection successful show IP address in serial monitor
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
*/
}
