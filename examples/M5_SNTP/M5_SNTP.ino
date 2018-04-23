#include <M5Stack.h>
#include <ArduinoJson.h>
#include <time.h>
#include <sys/time.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "nvs_flash.h"
#include "apps/sntp/sntp.h"
#include "qrcode.h"

#define CALENDARPAGE  0
#define GALLERY       1
#define QRCODE        2
#define wifi_ssid "--------"
#define wifi_password "--------"

File directory;
long interval, t1, t2;
String source;
uint16_t bground, currentPhoto = -1, photos = 0;
uint8_t frameNumber = 255, maxFrames = 3;

void savePrefs() {
  String prefs = "{\"source\": \"" + source + "\", \"interval\": " + String(interval) + ", \"bground\": " + String(bground) + "}";
  Serial.println("Saving preferences:");
  Serial.println(prefs);
  File file = SD.open("/GALLERY.TXT", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing!");
    return;
  }
  if (file.print(prefs)) {
    Serial.println("File written");
  }
  file.close();
}

void drawQRCode() {
  QRCode qrcode;
  Serial.print(F("Generating QR Code"));
  uint8_t version = 6;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  uint32_t dt = millis();
  qrcode_initText(&qrcode, qrcodeData, version, 0, "Kongduino|HONG KONG|http://kongduino.wordpress.com|KongDuino@ProtonMail.COM");
  dt = millis() - dt;
  Serial.println(F(" done."));
  Serial.print(F("QR Code Generation Time: "));
  Serial.println(dt);
  M5.Lcd.fillScreen(TFT_WHITE);
  uint8_t thickness = 220 / qrcode.size;
  uint16_t lineLength = qrcode.size * thickness;
  uint8_t xOffset = (320 - (lineLength)) / 2;
  uint8_t yOffset = (240 - (lineLength)) / 2;
  for (uint8_t y = 0; y < qrcode.size; y++) {
    // Left quiet zone
    Serial.print("        ");
    // Each horizontal module
    for (uint8_t x = 0; x < qrcode.size; x++) {
      // Print each module (UTF-8 \u2588 is a solid block)
      bool q = qrcode_getModule(&qrcode, x, y);
      Serial.print(q ? "\u2588\u2588" : "  ");
      if (q) {
        M5.Lcd.fillRect(x * thickness + xOffset, y * thickness + yOffset, thickness, thickness, TFT_BLACK);
      }
    }
    Serial.write('\n');
  }
  // Bottom quiet zone
  Serial.print("\n\n\n\n");
  M5.Lcd.setFreeFont(&FreeSans9pt7b);
  String s = "QR Code Generation Time: " + String(dt) + " ms";
  M5.Lcd.setTextColor(TFT_RED, TFT_WHITE);
  int16_t tw = M5.Lcd.textWidth(s);
  M5.Lcd.drawString(s, (320 - tw) / 2, 223);
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
}

void drawCalendarPage() {
  M5.Lcd.fillScreen(TFT_WHITE);
  time_t now;
  struct tm timeinfo;
  // http://www.cplusplus.com/reference/ctime/tm/
  // http://www.cplusplus.com/reference/ctime/strftime/
  time(&now);
  localtime_r(&now, &timeinfo);
  // Is time set? If not, tm_year will be (1970 - 1900).
  if (timeinfo.tm_year < (2018 - 1900)) {
    Serial.print(F("Time is not set yet. Connecting to WiFi and getting time over NTP.\n"));
    obtain_time();
    // update 'now' variable with current time
    time(&now);
  }
  char strftime_buf[64];
  // Set timezone to China Standard Time
  setenv("TZ", "CST-8", 1);
  tzset();
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%F [%a] %H:%M:%S", &timeinfo);
  Serial.printf("The current date/time in HK is: %s\n", strftime_buf);

  strftime(strftime_buf, sizeof(strftime_buf), "%b", &timeinfo);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.fillRoundRect(0, 0, 320, 60, 8, TFT_RED);
  M5.Lcd.fillRect(0, 0, 320, 5, TFT_RED);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_RED);
  M5.Lcd.drawRect(0, 0, 320, 240, TFT_BLACK);
  M5.Lcd.drawRect(1, 1, 318, 238, TFT_BLACK);

  int16_t tw = M5.Lcd.textWidth(strftime_buf);
  M5.Lcd.drawString(strftime_buf, (320 - tw) / 2, 8);

  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  String tmp = String(timeinfo.tm_mday);
  tw = M5.Lcd.textWidth(tmp);
  M5.Lcd.drawString(tmp, (320 - tw) / 2, 62);

  M5.Lcd.setFreeFont(&FreeSansBold12pt7b);
  strftime(strftime_buf, sizeof(strftime_buf), "%A", &timeinfo);
  tw = M5.Lcd.textWidth(strftime_buf);
  M5.Lcd.drawString(strftime_buf, (320 - tw) / 2, 110);

  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S", &timeinfo);
  tw = M5.Lcd.textWidth(strftime_buf);
  M5.Lcd.fillRect(2, 180, 316, 48, TFT_WHITE);
  //M5.Lcd.drawString(strftime_buf, (320 - tw) / 2, 180);
  M5.Lcd.drawString(strftime_buf, (300 - tw) / 2, 180, 7);
  delay(990);
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S", &timeinfo);
  tw = M5.Lcd.textWidth(strftime_buf);
  M5.Lcd.fillRect(2, 180, 316, 48, TFT_WHITE);
  //M5.Lcd.drawString(strftime_buf, (320 - tw) / 2, 180);
  M5.Lcd.drawString(strftime_buf, (300 - tw) / 2, 180, 7);
  delay(990);
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S", &timeinfo);
  tw = M5.Lcd.textWidth(strftime_buf);
  M5.Lcd.fillRect(2, 180, 316, 48, TFT_WHITE);
  //M5.Lcd.drawString(strftime_buf, (320 - tw) / 2, 180);
  M5.Lcd.drawString(strftime_buf, (300 - tw) / 2, 180, 7);
}

void drawGallery() {
  currentPhoto++;
  if (currentPhoto == photos) {
    currentPhoto = 0;
    directory.rewindDirectory();
  }
  M5.Lcd.fillScreen(bground);
  File file = directory.openNextFile();
  Serial.print("Displaying ");
  Serial.println(file.name());
  M5.Lcd.drawJpgFile(SD, file.name(), 0, 0, 320, 240);
}


void drawFrame(uint8_t frame) {
  switch (frame) {
    case CALENDARPAGE:
      drawCalendarPage();
      break;
    case GALLERY:
      drawGallery();
      break;
    case QRCODE:
      drawQRCode();
      break;
  }
}

boolean checkConnection() {
  int count = 0;
  Serial.print(F("Waiting for Wi-Fi connection"));
  M5.Lcd.drawString(F("Waiting for Wi-Fi"), 0, 37);
  while (count < 30) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected!");
      M5.Lcd.drawString(F("Connected to Wifi!"), 0, 37);
      M5.Lcd.drawString(F("         "), 0, 222);
      return (true);
    }
    delay(500);
    Serial.print(".");
    switch (count % 3) {
      case 0:
        M5.Lcd.drawString(F("[.  ]    "), 0, 222);
        break;
      case 1:
        M5.Lcd.drawString(F("[.. ]    "), 0, 222);
        break;
      case 2:
        M5.Lcd.drawString(F("[...]    "), 0, 222);
        break;
    }
    count++;
  }
  Serial.println("Timed out.");
  M5.Lcd.drawString(F("Timed out."), 100, 222);
  return false;
}

void parseDirectory() {
  directory = SD.open(source);
  if (!directory) {
    Serial.print(F("Failed to open directory: "));
    Serial.println(directory.name());
    while (1);
  }
  if (!directory.isDirectory()) {
    Serial.print(directory.name());
    Serial.println(F(": Not a directory"));
    while (1);
  }
  File file = directory.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print(F("Skipping directory "));
      Serial.println(file.name());
    } else {
      Serial.print(F(" • File: "));
      Serial.println(file.name());
      photos++;
    }
    file = directory.openNextFile();
  }
  file.close();
  Serial.print(F("Number of photos: "));
  Serial.println(photos);
  directory.rewindDirectory();
  currentPhoto = -1;
}

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  M5.begin();
  // Lcd display
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.setFreeFont(&FreeSansBold12pt7b);
  Serial.println("\n\ngetChipRevision(): " + String(ESP.getChipRevision()));
  Serial.println("getFlashChipSize(): " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB");
  Serial.println("getFlashChipSpeed(): " + String(ESP.getFlashChipSpeed() / 1024 / 1024) + " MHz");
  Serial.println("getCpuFreqMHz(): " + String(ESP.getCpuFreqMHz()) + " MHz");
  Serial.println("getFlashChipMode(): " + String(ESP.getFlashChipMode()));
  Serial.println("getSdkVersion(): " + String(ESP.getSdkVersion()));
  M5.Lcd.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.drawString(F("\n\nSNTP Test!"), 80, 0);
  Serial.print(F("WIFI-SSID: "));
  Serial.println(wifi_ssid);
  Serial.print(F("WIFI-PASSWD: "));
  Serial.println(wifi_password);

  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  bool x = false;
  while (!x) {
    Serial.write(' ');
    x = checkConnection();
  }
  Serial.println(WiFi.localIP());
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  // Is time set? If not, tm_year will be (1970 - 1900).
  if (timeinfo.tm_year < (2016 - 1900)) {
    Serial.print(F("Time is not set yet. Connecting to WiFi and getting time over NTP.\n"));
    obtain_time();
    // update 'now' variable with current time
    time(&now);
  }
  char strftime_buf[64];
  // Set timezone to China Standard Time
  setenv("TZ", "CST-8", 1);
  tzset();
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%F [%a] %H:%M:%S", &timeinfo);
  Serial.printf("The current date/time in HK is: %s\n", strftime_buf);
  M5.Lcd.drawString(F("Current date/time in HK:     "), 0, 62);
  M5.Lcd.drawString(strftime_buf, 0, 87);

  M5.Lcd.drawString(F("Gallery preferences"), 0, 110);
  File file = SD.open("/GALLERY.TXT");
  if (!file) {
    Serial.println(F("Failed to open file `/GALLERY.TXT` for reading!"));
    M5.Lcd.drawString(F("Failed to open preferences!"), 0, 107);
    while (1);
  }
  Serial.println(F("Read from file."));
  // {"source": "/GALLERY", "interval": 2000, "bground": 0}
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(file);
  if (!root.success()) {
    Serial.println(F("parseObject() failed"));
    M5.Lcd.drawString(F("parseObject() failed!     "), 0, 107);
    while (1);
  }
  interval = root["interval"];
  source = root["source"].as<String>();
  bground = root["bground"];
  file.close();

  Serial.print(F("source: "));
  Serial.println(source);
  Serial.print(F("interval: "));
  Serial.println(interval);
  Serial.print(F("bground: 0x"));
  Serial.println(bground, HEX);
  parseDirectory();

  server.begin();
  t1 = millis();
}

void obtain_time(void) {
  initialize_sntp();
  // wait for time to be set
  time_t now = 0;
  struct tm timeinfo = { 0 };
  int retry = 0;
  const int retry_count = 12;
  M5.Lcd.drawString(F("Waiting for SNTP time...   "), 0, 62);
  while (timeinfo.tm_year < (2018 - 1900) && ++retry < retry_count) {
    Serial.printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
    switch (retry % 3) {
      case 0:
        M5.Lcd.drawString(F("[.  ]    "), 0, 222);
        break;
      case 1:
        M5.Lcd.drawString(F("[.. ]    "), 0, 222);
        break;
      case 2:
        M5.Lcd.drawString(F("[...]    "), 0, 222);
        break;
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    time(&now);
    localtime_r(&now, &timeinfo);
  }
  M5.Lcd.drawString(F("SNTP time set...            "), 0, 62);
  M5.Lcd.drawString(F("         "), 0, 222);
}

void initialize_sntp(void) {
  Serial.println("Initializing SNTP");
  M5.Lcd.drawString(F("Initializing SNTP...   "), 0, 62);
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
}

String response = "";

void loop() {
  t2 = millis() - t1;
  if (t2 >= interval) {
    // We display a new frame.
    frameNumber++;
    if (frameNumber == maxFrames) frameNumber = 0;
    Serial.print("Drawing frame: ");
    Serial.println(frameNumber);
    drawFrame(frameNumber);
    t1 = millis();
  }
  WiFiClient client = server.available(); // listen for incoming clients
  if (client) { // if you get a client,
    Serial.println("New Client."); // print a message out the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there are bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            // the content of the HTTP response follows the header:
            client.print("");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else { // if you got a newline, then clear currentLine:
            if (currentLine.startsWith("GET /interval=")) {
              uint16_t intvl = 0;
              uint8_t ln = currentLine.length(), pos = 14;
              char buf[ln];
              currentLine.toCharArray(buf, ln);
              char c = buf[pos++];
              while (c >= '0' && c <= '9') {
                intvl = intvl * 10 + (c - 48);
                c = buf[pos++];
              }
              if (intvl < 100) intvl = intvl * 1000;
              else if (intvl > 999) interval = intvl;
              else intvl = interval;
              interval = intvl;
              // a number lower than 100 should mean an interval in seconds
              // whereas a number higher than 999 (ie 1000+) should me milliseconds
              // between 100 and 999? No clue
              Serial.print("Setting interval to: ");
              Serial.print(interval); Serial.println(" ms.");
              savePrefs();
            } if (currentLine.startsWith("GET /source=")) {
              String newSource = "";
              uint8_t ln = currentLine.length(), pos = 12;
              char buf[ln];
              currentLine.toCharArray(buf, ln);
              char c = buf[pos++];
              while (c != ' ') {
                newSource += c;
                c = buf[pos++];
              }
              source = newSource;
              // a number lower than 100 should mean an interval in seconds
              // whereas a number higher than 999 (ie 1000+) should me milliseconds
              // between 100 and 999? No clue
              Serial.print("Setting source to: ");
              Serial.print(source); Serial.println(" ms.");
              parseDirectory();
              savePrefs();
            }
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}