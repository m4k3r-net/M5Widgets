#include <Arduino.h>
#include <M5Stack.h>
#include "Free_Fonts.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "M5StackUpdater.h"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define myUUID "B6C3-C682-8E6B-54E5"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

std::string rxValue;
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        String s = "";
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
          s = s + (char)rxValue[i];
        }
        Serial.println();
        Serial.println("*********");
        M5.Lcd.fillRect(0, 100, 320, 144, TFT_WHITE);
        M5.Lcd.drawString(s, 0, 100, GFXFF);
      }
    }
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  M5.begin();
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.setFreeFont(FSSB12);
  // Select the font: FreeMono18pt7b – see Free_Fonts.h
  M5.Lcd.drawString(F("BLE UART Test"), 85, 18, GFXFF);
  M5.Lcd.setFreeFont(FSS9); // FreeSans9pt7b
  Serial.println("\n Setting up Bluetooth..");
  // Create the BLE Device
  BLEDevice::init("BMP UART Service");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_READ || BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  // Start the service
  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  M5.Lcd.drawString("BLE [o]", 0, 18, GFXFF);
  M5.Lcd.drawString(SERVICE_UUID, 0, 80, GFXFF);
  Serial.println("Waiting a client connection to notify..");
}

void loop() {
  if (deviceConnected) {
    // Serial.printf("We should send back something..");
    pCharacteristic->setValue(rxValue);
    pCharacteristic->notify();
  }
}