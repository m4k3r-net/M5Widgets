//#define HAVE_STACKUPDATER
// Use this define if you have M5StackUpdater installed
// This will make the sketch M5StackUpdater compatible
#include <M5Stack.h>
#include <M5Widget.h>
#ifdef HAVE_STACKUPDATER
#include "M5StackUpdater.h"
#endif

#define FF22 &FreeSansBold12pt7b
#define FF18 &FreeSans12pt7b
#define FF20 &FreeSans24pt7b
#define FSSB9 &FreeSansBold9pt7b

M5ProgressBar r;
M5ProgressBar g;
M5ProgressBar b;

void demoColour() {
  uint8_t vR = r.value(), vG = g.value(), vB = b.value();
  uint16_t clr = M5.Lcd.color565(vR, vG, vB);
  uint16_t reverse = clr ^ 0xFFFF;
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setTextColor(reverse, clr);
  M5.Lcd.fillRoundRect(20, 20, 280, 160, 3, clr);
  M5.Lcd.setFreeFont(FF22); // Select the font
  String s = String(clr, HEX);
  while (s.length() < 4) s = "0" + s;
  s = "0x" + s;
  M5.Lcd.drawString(s, 160, 60, 1);
  M5.Lcd.setFreeFont(FSSB9); // Select the font
  s = String(vR, HEX);
  if (s.length() < 2) s = "0" + s;
  s = "0x" + s;
  M5.Lcd.drawString(s, 60, 90, 1);
  M5.Lcd.drawString("Red", 60, 110, 1);
  s = String(vG, HEX);
  if (s.length() < 2) s = "0" + s;
  s = "0x" + s;
  M5.Lcd.drawString(s, 160, 90, 1);
  M5.Lcd.drawString("Green", 160, 110, 1);
  s = String(vB, HEX);
  if (s.length() < 2) s = "0" + s;
  s = "0x" + s;
  M5.Lcd.drawString(s, 260, 90, 1);
  M5.Lcd.drawString("Blue", 260, 110, 1);
}

void setup() {
  Serial.begin(115200);
  M5.begin();
#ifdef HAVE_STACKUPDATER
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
#endif
  Serial.println(""); Serial.println("");
  Serial.println("COLOURS test!");
  M5.Lcd.init();
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.fillScreen(TFT_WHITE);
  r.setBounds(0, 200, 100, 22);
  g.setBounds(110, 200, 100, 22);
  b.setBounds(220, 200, 100, 22);
  r.setColor(BACKGROUND, TFT_WHITE);
  r.setColor(FRAME, TFT_ORANGE);
  r.setColor(CONTENT, TFT_RED);
  r.setColor(FOREGROUND, TFT_BLACK);
  r.setFont(FSSB9);
  g.setColor(BACKGROUND, TFT_WHITE);
  g.setColor(FRAME, TFT_DARKGREEN2);
  g.setColor(CONTENT, TFT_GREEN);
  g.setColor(FOREGROUND, TFT_BLACK);
  g.setFont(FSSB9);
  b.setColor(BACKGROUND, TFT_WHITE);
  b.setColor(FRAME, TFT_BLACK);
  b.setColor(CONTENT, TFT_BLUE);
  b.setColor(FOREGROUND, TFT_BLACK);
  b.setFont(FSSB9);
  r.setRange(0, 255);
  g.setRange(0, 255);
  b.setRange(0, 255);
  r.disablePrint();
  g.disablePrint();
  b.disablePrint();
  r.setValue(255.0);
  g.setValue(0.0);
  b.setValue(0.0);
  r.draw();
  g.draw();
  b.draw();
  demoColour();
}

void loop() {
  M5.update();
  if (M5.BtnA.isPressed()) {
    while (M5.BtnA.isPressed()) {
      uint8_t x = r.value() + 1;
      r.setValue(x);
      r.draw();
      delay(100);
      M5.update();
      demoColour();
    }
  }

  if (M5.BtnB.isPressed()) {
    while (M5.BtnB.isPressed()) {
      uint8_t x = g.value() + 1;
      g.setValue(x);
      g.draw();
      delay(100);
      M5.update();
      demoColour();
    }
  }

  if (M5.BtnC.isPressed()) {
    while (M5.BtnC.isPressed()) {
      uint8_t x = b.value() + 1;
      b.setValue(x);
      b.draw();
      delay(100);
      M5.update();
      demoColour();
    }
  }
}
