#include <M5Stack.h>
#include <M5Widget.h>

M5Mosaic mm;
#define FF22 &FreeSansBold12pt7b
#define FF21 &FreeSansBold9pt7b
#define FF17 &FreeSans9pt7b

uint32_t t0;
uint8_t nbBTN = 8;

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.begin();
  M5.lcd.setBrightness(200);
  MosaicPiece piece;
  for (uint8_t i = 0; i < nbBTN; i++) {
    uint8_t ix = i;
    std::string name = "B   ";
    if (ix < 10) name[3] = (char)(ix + 48);
    else {
      ix /= 10;
      name[2] = (char)(ix + 48);
      ix = i - (ix*10);
      name[3] = (char)(ix + 48);
    }
    Serial.println("name: " + String(name.c_str()));
    piece.name = name;
    if (ix % 2 == 0) {
      piece.bgColor = TFT_LIGHTSALMON;
      piece.fgColor = TFT_SEAGREEN;
      piece.square = true;
    } else {
      piece.bgColor = TFT_GREEN;
      piece.fgColor = TFT_YELLOW;
      piece.square = false;
    }
    piece.enabled = true;
    mm.addValue(piece);
  }
  mm.setBounds(0, 0, 320, 200);
  mm.setColor(BACKGROUND, TFT_BLACK);
  mm.setColor(FRAME, TFT_BLACK);
  mm.setColor(FOREGROUND, TFT_NAVY);
  mm.setColor(CONTENT, TFT_CYAN);
  mm.setColor(HIGHLIGHT, TFT_RED);
  mm.setColor(ACTIVATED, TFT_BLUE);

  mm.setFont(FF21);
  mm.setName("M5mosaic");

  mm.clear();
  t0 = millis();
  Serial.println("\n\n\nsetup done...");
}

void loop() {
  int8_t ix = mm.feedback();
  if (ix > -1) {
    M5.Lcd.setTextColor(mm.color(FOREGROUND), mm.color(CONTENT));
    M5.Lcd.setFont(FF21);
    Serial.println("getPieceName[" + String(ix) + "]: " + String(mm.getPieceName(ix).c_str()));
    M5.Lcd.drawString("You pressed " + String(mm.getPieceName(ix).c_str()), 100, 8, 1);
    delay(1000);
    mm.clear();
  }
}