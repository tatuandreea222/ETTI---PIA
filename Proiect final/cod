#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

//This code is released under the MIT license.
//Please check the license before redistributing.

//Build Check
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//Constants
//Use https://www.uuidgenerator.net/ to get a random UUID.
class Constants{
public:
  String ssid = "Tătu";
  String password = "seminte22";
  String bleServerName = "Proiect_IA_Low_Energy";
  String service_uuid = "117178da-16d9-4c83-a248-1e0039b7f319";
  String apiListURL = "http://proiectia.bogdanflorea.ro/api/board-game-atlas/games";
  String apiFetchURL = "http://proiectia.bogdanflorea.ro/api/board-game-atlas/game?ids=";
  String idProperty = "id";
  String nameProperty = "name";
  String imageProperty = "image_url";
  String year = "year_published";
  String descriptionProperty = "price_text";
  bool deviceConnected = false;
};
Constants *constants = new Constants();

BLECharacteristic indexCharacteristic(
  constants->service_uuid.c_str(),
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
);
BLECharacteristic detailsCharacteristic(
  constants->service_uuid.c_str(),
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
);
BLEDescriptor *indexDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor *detailsDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902));

//Classes
class Utils{
public:
  static DynamicJsonDocument fetchJson(String url){
      DynamicJsonDocument document(8096);
      HTTPClient http;
      String payload;

      http.setTimeout(15000);
      http.begin(url);
      http.GET();
      payload = http.getString();
      deserializeJson(document, payload);

      return document;
  }

  static DynamicJsonDocument createListDocumentJson(JsonObject object){
      DynamicJsonDocument listDocumentJson(8096);

      listDocumentJson["id"] = object[constants->idProperty];
      listDocumentJson["name"] = object[constants->nameProperty];
      listDocumentJson["image"] = object[constants->imageProperty];
     
      return listDocumentJson;
  }

  static DynamicJsonDocument createDetailsDocumentJson(DynamicJsonDocument document){
      DynamicJsonDocument detailsDocumentJson(8096);

      detailsDocumentJson["id"] = document[constants->idProperty];
      detailsDocumentJson["name"] = document[constants->nameProperty];
      detailsDocumentJson["image"] = document[constants->imageProperty];
      detailsDocumentJson["description"] = document[constants->descriptionProperty].as<String>() + ", " + document[constants->year].as<String>();
      
      return detailsDocumentJson;
  }

  static void sendJson(DynamicJsonDocument document, BLECharacteristic *characteristic){
      String returned;
      serializeJson(document, returned);
      
      characteristic->setValue(returned.c_str());
      characteristic->notify();
  }
};

class WiFiConnection{
protected:
  String ssid;
  String password;
public:
  WiFiConnection(String ssid, String password, bool begin = true) :
                  ssid(ssid), password(password){
    if(begin){
      this->connect();  
    }
  }
  
  void setSSID(String ssid){
    this->ssid = ssid;
  }
  void setPassword(String password){
    this->password = password;  
  }

  String getSSID(){
    return this->ssid;
  }
  
  void connect(){
    WiFi.begin(this->ssid.c_str(), this->password.c_str());
  }
  bool isConnected(){
    if(WiFi.status() == WL_CONNECTED){
      return true;
    }

    return false;
  }
};
WiFiConnection* WiFiInstance;

class CharacteristicsCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      DynamicJsonDocument appRequest(8096);
      deserializeJson(appRequest, characteristic->getValue().c_str());
    
      if(appRequest["action"] == "fetchData"){
        DynamicJsonDocument webJSON = Utils::fetchJson(constants->apiListURL);
         for(JsonObject object : webJSON.as<JsonArray>()){
          DynamicJsonDocument returnJSON = Utils::createListDocumentJson(object);
          Utils::sendJson(returnJSON, characteristic);
         }
      }else if(appRequest["action"] == "fetchDetails"){
        DynamicJsonDocument webJSON = Utils::fetchJson(constants->apiFetchURL + appRequest["id"].as<String>());
        DynamicJsonDocument returnJSON = Utils::createDetailsDocumentJson(webJSON);
        Utils::sendJson(returnJSON, characteristic);
      }
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    constants->deviceConnected = true;
    Serial.println("Device connected");
  };
  void onDisconnect(BLEServer* pServer) {
    constants->deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

class BluetoothConnection{
protected:
  BLEServer *pServer;
  BLEService *bmeService;
  BLEAdvertising *pAdvertising;
  String name;
public:
  BluetoothConnection(String name, bool begin = true, bool alert = true) : name(name) {
    if(begin){
      this->tryPair(alert);  
    }
  }

  void tryPair(bool alert = false){
    BLEDevice::init(this->name.c_str());
    
    this->pServer = BLEDevice::createServer();
    this->pServer->setCallbacks(new MyServerCallbacks());

    this->bmeService = this->pServer->createService(constants->service_uuid.c_str());
    this->bmeService->addCharacteristic(&indexCharacteristic);
    indexDescriptor->setValue("Get data list");
    indexCharacteristic.addDescriptor(indexDescriptor);
    indexCharacteristic.setValue("Get data List");

    indexCharacteristic.setCallbacks(new CharacteristicsCallbacks());

    this->bmeService->addCharacteristic(&detailsCharacteristic);  
    detailsDescriptor->setValue("Get data details");
    detailsCharacteristic.addDescriptor(detailsDescriptor);
    detailsCharacteristic.setValue("Get data details");
    
    detailsCharacteristic.setCallbacks(new CharacteristicsCallbacks());

    this->bmeService->start();

    this->pAdvertising = BLEDevice::getAdvertising();
    this->pAdvertising->addServiceUUID(constants->service_uuid.c_str());
    this->pServer->getAdvertising()->start();
    
    if(alert){
      Serial.println("Waiting a client connection to notify...");  
    }
  }
};
BluetoothConnection* BluetoothInstance;

void setup() {
  Serial.begin(115200);
  
  WiFiInstance = new WiFiConnection(constants->ssid, constants->password);
  BluetoothInstance = new BluetoothConnection(constants->bleServerName);
}

void loop() {
  //Empty
}