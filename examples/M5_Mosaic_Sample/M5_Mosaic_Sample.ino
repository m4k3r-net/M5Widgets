#include <M5Stack.h>
#include <M5Widget.h>

M5Mosaic mm;
#define FF22 &FreeSansBold12pt7b
#define FF21 &FreeSansBold9pt7b

uint32_t t0;

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.begin();
  M5.lcd.setBrightness(200);
  MosaicPiece piece;
  std::string name = "B 1";
  piece.name = name;
  piece.index = 0;
  piece.bgColor = TFT_LIGHTSALMON;
  piece.fgColor = TFT_SEAGREEN;
  piece.enabled = true;
  piece.square = true;
  mm.addValue(piece);

  name = "B 2";
  piece.name = name;
  piece.index = 1;
  piece.bgColor = TFT_LIGHTBLUE;
  piece.fgColor = TFT_YELLOW;
  piece.enabled = false;
  piece.square = true;
  mm.addValue(piece);

  name = "B 3";
  piece.name = name;
  piece.index = 2;
  piece.bgColor = TFT_GREEN;
  piece.fgColor = TFT_YELLOW;
  piece.enabled = true;
  piece.square = true;
  mm.addValue(piece);

  name = "B 4";
  piece.name = name;
  piece.index = 3;
  piece.bgColor = TFT_LIGHTSLATEGRAY;
  piece.fgColor = TFT_PAPAYAWHIP;
  piece.enabled = true;
  piece.square = true;
  mm.addValue(piece);

  name = "B 5";
  piece.name = name;
  piece.index = 3;
  piece.bgColor = TFT_LIGHTGREEN;
  piece.fgColor = TFT_BROWN;
  piece.enabled = true;
  piece.square = true;
  mm.addValue(piece);

  name = "B 6";
  piece.name = name;
  piece.index = 3;
  piece.bgColor = TFT_LIGHTCORAL;
  piece.fgColor = TFT_MEDIUMPURPLE;
  piece.enabled = true;
  piece.square = true;
  mm.addValue(piece);

  name = "B 7";
  piece.name = name;
  piece.index = 3;
  piece.bgColor = TFT_PEACHPUFF;
  piece.fgColor = TFT_SEAGREEN;
  piece.enabled = true;
  piece.square = true;
  mm.addValue(piece);

  mm.setBounds(0, 0, 320, 200);
  mm.setColor(BACKGROUND, TFT_BLACK);
  mm.setColor(FRAME, TFT_WHITE);
  mm.setColor(FOREGROUND, TFT_NAVY);
  mm.setColor(CONTENT, TFT_CYAN);
  mm.setColor(HIGHLIGHT, TFT_RED);
  mm.setColor(ACTIVATED, TFT_BLUE);

  mm.setFont(FF21);

  mm.clear();
  t0 = millis();
  Serial.println("\n\n\nsetup done...");
}

void loop() {
  int8_t ix = mm.feedback();
  Serial.println("feedback = " + String(ix));
  if (ix > -1) {
    M5.Lcd.drawString("You pressed " + String(mm.getPieceName(ix).c_str()), 20, 1, 1);
    delay(1000);
    mm.clear();
  }
}

