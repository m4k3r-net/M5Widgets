#include <M5Stack.h>
#include "Free_Fonts.h"

uint16_t countFieldsB(char *myString, char delim) {
  uint16_t length = 1;
  uint8_t i = 0, pos = 0;
  i = myString[pos++];
  while (i > 0) {
    if (i == delim) {
      length++;
    }
    i = myString[pos++];
  }
  return length;
}

void splitB(char *myString, uint16_t * table, char delim) {
  // You need to use countFieldsB first to ascertain the number of elements
  uint16_t length = 1;
  uint8_t i = 0, pos = 0;
  i = myString[pos];
  table[0] = 0;
  while (i > 0) {
    if (i == delim) {
      table[length++] = pos;
    }
    pos++;
    i = myString[pos];
  }
  table[length] = pos;
}

String Title = "My Menu";
char MainMenu[] = "Menu 1|Menu 2 [quite longer...]|Menu 3 [mid-size]";
char SubMenu1[] = "Menu 1-1,Menu 1-2";
char SubMenu2[] = "Menu 2-1,Menu 2-2,Menu 2-3";
char SubMenu3[] = "Menu 3-1,Menu 3-2, Menu 3-3, Menu 3-4";

class menuCallbacks;
class menuCallbacks {
  public:
    virtual ~menuCallbacks() {};
    virtual void onEnterMenu(uint8_t level, uint8_t index);
    virtual void onLeaveMenu(uint8_t level, uint8_t index);
};

class MyCallbacks: public menuCallbacks {
    void onEnterMenu(uint8_t level, uint8_t index) {
    };
    void onLeaveMenu(uint8_t level, uint8_t index) {
    }
};

menuCallbacks* m_Callbacks = nullptr;

// m_Callbacks->onConnect(this);

void drawMainMenu(int8_t index = -1) {
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.setFreeFont(FSSB12);
  M5.Lcd.fillRect(0, 50, 320, 240, TFT_WHITE);
  // Select the font: FreeMono18pt7b – see Free_Fonts.h
  int16_t w = M5.Lcd.textWidth(Title);
  uint8_t px = (320 - w) / 2;
  M5.Lcd.drawString(Title, px, 18, GFXFF);
  Serial.println("Drawing \"" + Title + "\" at " + String(px));
  M5.Lcd.setFreeFont(FSS12); // FreeSans9pt7b
  m_Callbacks = new MyCallbacks();
  uint16_t cnt = countFieldsB(MainMenu, '|');
  Serial.println(String(cnt) + " main menu items");
  uint16_t py = 50, table[cnt], maxWidth = 0;
  uint8_t i, n = 0, x, y, t;
  splitB(MainMenu, table, '|');
  for (i = 0; i < cnt; i++) {
    y = table[n + 1];
    String s = "";
    t = table[n];
    if (n > 0) t++;
    for (x = t; x < y; x++) {
      s = s + (char)MainMenu[x];
    }
    w = M5.Lcd.textWidth(s);
    if (w > maxWidth) maxWidth = w;
    py += 30;
    n++;
  }
  py = 50;
  for (i = 0; i < cnt; i++) {
    y = table[n + 1];
    String s = "";
    t = table[n];
    if (n > 0) t++;
    for (x = t; x < y; x++) {
      s = s + (char)MainMenu[x];
    }
    if (index == i) {
      M5.Lcd.fillRect(0, py - 3, maxWidth + 10, 28, TFT_PURPLE);
      M5.Lcd.setTextColor(TFT_WHITE);
      M5.Lcd.drawString(s, 5, py, GFXFF);
      M5.Lcd.drawRect(0, py - 3, maxWidth + 10, 28, TFT_WHITE);
    } else {
      M5.Lcd.fillRect(0, py - 3, maxWidth + 10, 28, TFT_WHITE);
      M5.Lcd.setTextColor(TFT_BLACK);
      M5.Lcd.drawString(s, 5, py, GFXFF);
      M5.Lcd.drawRect(0, py - 3, maxWidth + 10, 28, TFT_PURPLE);
    }
    py += 30;
    n++;
  }
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  M5.begin();
  drawMainMenu(0);
}

void loop() {
}
