#include <ArduinoJson.h>
#include <M5Stack.h>
#include "M5StackUpdater.h"

long interval, t1, t2;
String source;
uint16_t bground, currentPhoto = -1, photos = 0;
File directory;

void setup() {
  Serial.begin(115200);
  M5.begin();
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  M5.Lcd.begin();
  Serial.println();
  Serial.println();
  Serial.println("Gallery");
  Serial.println();
  Serial.println("Importing Preferences");
  File file = SD.open("/GALLERY.TXT");
  if (!file) {
    Serial.println("Failed to open file `/GALLERY.TXT` for reading!");
    while (1);
  }
  Serial.println("Read from file.");
  // {"source": "/GALLERY", "interval": 2000, "bground": 0}
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(file);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    while (1);
  }
  interval = root["interval"];
  source = root["source"].as<String>();
  bground = root["bground"];
  M5.Lcd.fillScreen(bground);
  file.close();

  Serial.print("source: ");
  Serial.println(source);
  Serial.print("interval: ");
  Serial.println(interval);
  Serial.print("bground: 0x");
  Serial.println(bground, HEX);
  directory = SD.open(source);
  if (!directory) {
    Serial.print("Failed to open directory: ");
    Serial.println(directory.name());
    while (1);
  }
  if (!directory.isDirectory()) {
    Serial.print(directory.name());
    Serial.println(": Not a directory");
    while (1);
  }
  file = directory.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("Skipping directory ");
      Serial.println(file.name());
    } else {
      Serial.print("File: ");
      Serial.print(file.name());
      photos++;
    }
    file = directory.openNextFile();
  }
  file.close();
  Serial.println("Number of photos: " + String(photos));
  directory.rewindDirectory();
  t1 = millis();
}

void loop() {
  t2 = millis() - t1;
  if (t2 >= interval) {
    currentPhoto++;
    if (currentPhoto == photos) {
      currentPhoto = 0;
      directory.rewindDirectory();
    }
    File file = directory.openNextFile();
    Serial.print("Displaying ");
    Serial.println(file.name());
    M5.Lcd.drawJpgFile(SD, file.name(), 0, 0, 320, 240);
    t1 = millis();
  }
}

