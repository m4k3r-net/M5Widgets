#include <M5Stack.h>
#include <M5Widget.h>
#include "Free_Fonts.h"
#include "esp_spi_flash.h"

M5Touch touch;
M5StaticMenu sm;
M5Bridge m5bridge;

#define HEADERHEIGHT 30
#define FOOTERHEIGHT 28
uint16_t centerTop, centerHeight, centerDrawTop;
String cenTerTitle = "SYS";
uint16_t bl;
M5BarGraph bar;

void setup() {
  Serial.begin(115200);
  delay(2000);
  M5.begin();
  centerTop = HEADERHEIGHT + 7;
  centerHeight = 240 - (HEADERHEIGHT + FOOTERHEIGHT);
  centerDrawTop = centerTop + HEADERHEIGHT;
  bl = 200; M5.lcd.setBrightness(bl);
  Serial.println("\n\n\n\nM5.begin()");
  // Lcd display
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.fillScreen(TFT_BLACK);
  sm.setDrawHeader(drawHeader);
  sm.setDrawCenter(drawCenter);
  sm.setDrawFooter(drawMainFooter);
  sm.draw();
  displayBattery();

  bar.setBounds(145, centerTop + HEADERHEIGHT + 10, 30, centerHeight - HEADERHEIGHT - FOOTERHEIGHT - 20);
  bar.setColor(ACTIVATED, TFT_BLUE);
  bar.setColor(BACKGROUND, TFT_WHITE);
  bar.setColor(CONTENT, TFT_YELLOW);
  bar.setColor(FOREGROUND, TFT_BLUE);
  bar.setColor(FRAME, TFT_BLACK);
  bar.setRange(0, 200);
}

void loop() {
  touch.tm.run();
}

void i2cScan() {
  // ripped from M5Stack-SAM
  byte error, address;
  int nDevices;
  Serial.println(F("Scanning..."));
  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    } else if (error == 4) {
      Serial.print(F("Unknown error at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) Serial.println(F("No I2C devices found\n"));
  else Serial.println(F("done\n"));
}

void displayBacklight() {
  cenTerTitle = "BACKLIGHT";
  sm.setDrawFooter(drawBLFooter);
  sm.draw();
  bar.setValue(bl);
  bar.draw();
}

/*
  #define CHIP_FEATURE_EMB_FLASH      BIT(0)
  #define CHIP_FEATURE_WIFI_BGN       BIT(1)
  #define CHIP_FEATURE_BLE            BIT(4)
  #define CHIP_FEATURE_BT             BIT(5)
  typedef struct {
    esp_chip_model_t model;  //!< chip model, one of esp_chip_model_t
    uint32_t features;       //!< bit mask of CHIP_FEATURE_x feature flags
    uint8_t cores;           //!< number of CPU cores
    uint8_t revision;        //!< chip revision number
  } esp_chip_info_t;

  typedef enum {
    CHIP_ESP32 = 1, //!< ESP32
  } esp_chip_model_t;
*/

void displayBattery() {
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf(
    "This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
    chip_info.cores,
    (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
    (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : ""
  );
  printf("silicon revision %d, ", chip_info.revision);
  printf(
    "%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
    (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external"
  );
  uint8_t chipRev = ESP.getChipRevision();
  uint8_t cpuSpeed = ESP.getCpuFreqMHz();
  uint8_t flashSize = ESP.getFlashChipSize() / 1024 / 1024;
  uint8_t flashSpeed = ESP.getFlashChipSpeed() / 1E6;
  //  const char * sdkVer = ESP.getSdkVersion();
  cenTerTitle = ESP.getSdkVersion();
  drawCenter();

  String s;
  M5.Lcd.setFont(FF22);
  M5.Lcd.setTextColor(TFT_OLIVE);
  M5.Lcd.drawString("CH7", 16, centerDrawTop + 8, 1);
  M5.Lcd.drawString("CH0", 260, centerDrawTop + 8, 1);
  M5.Lcd.setFont(FF18);
  uint16_t tw = M5.Lcd.textWidth(s);
  M5.Lcd.drawString(s, 320 - 8 - tw, centerDrawTop + 33, 1);

  s =  String(chip_info.cores) + " core";
  if (chip_info.cores > 1) s = s + "s";
  s = s + " @ " + String(cpuSpeed) + "MHz, rev " + String(chip_info.revision);
  M5.Lcd.drawString(s, 10, centerDrawTop + 63, 1);
  s =  "Flash: " + String(flashSpeed) + "MHz, " + String(flashSize) + "MB";
  if (chip_info.features & CHIP_FEATURE_EMB_FLASH == 0) s = s + "[EXT]";
  M5.Lcd.drawString(s, 10, centerDrawTop + 88, 1);
  s =  "Heap size: " + String(esp_get_free_heap_size() / 1024 ) + "KB";
  M5.Lcd.drawString(s, 10, centerDrawTop + 112, 1);
}

void drawHeader() {
  M5.Lcd.fillRect(0, 0, 320, HEADERHEIGHT, TFT_NAVY);
  M5.Lcd.setFont(FF21);
  M5.Lcd.setTextColor(TFT_CYAN);
  M5.Lcd.drawString("STATUS", 1, HEADERHEIGHT - 14, 1);
  M5.Lcd.setFont(FF17);
  M5.Lcd.drawString("system", 1, 0, 1);
}

void drawCenter() {
  M5.Lcd.fillRoundRect(3, centerTop, 314, centerHeight - 14, 4, TFT_WHITE);
  M5.Lcd.fillRoundRect(3, centerTop, 314, HEADERHEIGHT, 4, TFT_CYAN);
  M5.Lcd.fillRect(3, centerTop + HEADERHEIGHT - 4, 314, 4, TFT_WHITE);
  M5.Lcd.setFont(FF26);
  uint16_t tw = M5.Lcd.textWidth(cenTerTitle);
  uint16_t px = (M5.Lcd.width() / 2) - (tw / 2);
  M5.Lcd.setTextColor(TFT_NAVY);
  M5.Lcd.drawString(cenTerTitle, px, HEADERHEIGHT + 9, 1);
}

void drawMainFooter() {
  M5.Lcd.setFont(FF18);
  M5.Lcd.fillRoundRect(31, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_BLUE); // BtnA
  M5.Lcd.fillRoundRect(126, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_OLIVE); // BtnB
  M5.Lcd.fillRoundRect(221, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_DARKGREEN); // BtnC
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.drawCentreString("SYS", 31 + 30, 240 - FOOTERHEIGHT + 6, 2);
  M5.Lcd.drawCentreString("sleep", 126 + 30, 240 - FOOTERHEIGHT + 6, 2);
  M5.Lcd.drawCentreString("BL", 221 + 30, 240 - FOOTERHEIGHT + 6, 2);
  touch.setTouchFunctionA(displayBattery);
  touch.setTouchFunctionC(displayBacklight);
  touch.setTouchFunctionB(manageSleep); // instead of displayRSSI for the moment
}

void manageSleep() {
  M5.setWakeupButton(BUTTON_B_PIN);
  M5.powerOFF();
}

void drawBLFooter() {
  M5.Lcd.setFont(FF18);
  M5.Lcd.fillRoundRect(31, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_OLIVE); // BtnA
  M5.Lcd.fillRoundRect(126, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_RED); // BtnB
  M5.Lcd.fillRoundRect(221, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_DARKGREEN); // BtnC
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.drawCentreString("+", 31 + 30, 240 - FOOTERHEIGHT + 6, 2);
  M5.Lcd.drawCentreString("MAIN", 126 + 30, 240 - FOOTERHEIGHT + 6, 2);
  M5.Lcd.drawCentreString("-", 221 + 30, 240 - FOOTERHEIGHT + 6, 2);
  touch.setTouchFunctionA(increaseBacklight);
  touch.setTouchFunctionB(drawMainFooter);
  touch.setTouchFunctionC(decreaseBacklight);
}

void increaseBacklight() {
  bl += 10;
  if (bl > 200) bl = 200;
  M5.lcd.setBrightness(bl);
  bar.setValue(bl);
  bar.draw();
}

void decreaseBacklight() {
  if (bl < 10) bl = 0;
  else bl -= 10;
  M5.lcd.setBrightness(bl);
  bar.setValue(bl);
  bar.draw();
}

