#include "bthandler.h"

BLECharacteristic *pCharacteristic;

bool bt_connected;

bool is_bt_connected(){
  return bt_connected;
}


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      bt_connected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      bt_connected = false;
    }
};

void bt_send_data(uint8_t* data, size_t length){
  if(bt_connected){
    pCharacteristic->setValue(data, length);
    pCharacteristic->notify();
    Serial.println("bt message sent");
  }
}

void setup_bluetooth(){
  // Create the BLE Device
  BLEDevice::init("OhSnap");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}
