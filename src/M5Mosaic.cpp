#include "M5Mosaic.h"

void M5Mosaic::clear() {
  _lastCheck = millis();
  // Draw the frame
  M5.Lcd.fillRect(0, 0, 320, 240, color(BACKGROUND));
  M5.Lcd.fillRoundRect(x, y, width, height, 3, color(FRAME));

  // Draw the header
  M5.Lcd.fillRoundRect(0, 0, 320, HEADERHEIGHT, 3, color(CONTENT));
  M5.Lcd.setFont(FF21);
  M5.Lcd.setTextColor(color(FOREGROUND));
  M5.Lcd.drawString(_name, 1, HEADERHEIGHT /2, 1);
  M5.Lcd.setFont(FF17);
  M5.Lcd.drawString("test", 8, 0, 1);
  M5.Lcd.setFont(FF18);
  M5.Lcd.fillRoundRect(31, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_BLUE); // BtnA
  M5.Lcd.fillRoundRect(126, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_OLIVE); // BtnB
  M5.Lcd.fillRoundRect(221, 240 - FOOTERHEIGHT, 60, FOOTERHEIGHT, 3, TFT_DARKGREEN); // BtnC
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.drawCentreString("<", 31 + 30, 240 - FOOTERHEIGHT + 6, 2);
  M5.Lcd.drawCentreString("ok", 126 + 30, 240 - FOOTERHEIGHT + 6, 2);
  M5.Lcd.drawCentreString(">", 221 + 30, 240 - FOOTERHEIGHT + 6, 2);
  draw();
}

void M5Mosaic::update() {
  draw();
}

void M5Mosaic::setName(String s) {
  _name = s;
}

String M5Mosaic::getName() {
  return _name;
}

void M5Mosaic::draw() {
  if (!_inited) {
    if (height > 240 - HEADERHEIGHT - FOOTERHEIGHT) height = 240 - HEADERHEIGHT - FOOTERHEIGHT - 2;
    if (y < HEADERHEIGHT) y = HEADERHEIGHT + 1;
    if (width > 320 - x) width = 320 - x;
    if (_count < 4) {
      side = (width / 3);
      _row = 3;
      xOffset = (width - (side * 3)) / 2;
      yOffset = (height - side) / 2;
      M5.Lcd.setFont(FF22);
    } else if (_count < 7) {
      _row = 3;
      side = (width / 3);
      xOffset = (width - (side * 3)) / 2;
      yOffset = (height - (side * 2)) / 2;
      M5.Lcd.setFont(FF22);
    } else {
      // more than two lines
      M5.Lcd.setFont(FF21);
      uint8_t nLines = _count / 3;
      if (nLines * 3 < _count) nLines += 1;
      side = (height / nLines);
      _row = width / side;
      side = width / _row;
      nLines = _count / _row;
      xOffset = (width - (side * _row)) / 2;
      yOffset = (height - (side * nLines)) / 5;
    }
    Serial.println("\n\nside: " + String(side));
    Serial.println("_row: " + String(_row));
    Serial.println("xOffset: " + String(xOffset));
    Serial.println("yOffset: " + String(yOffset));
  }
  posX = x + xOffset;
  posY = y + yOffset;
  for (uint8_t i = 0; i < _count; i++) {
    MosaicPiece piece = _pieces[i];
    if (piece.square) {
      if (_selectedIndex == i) {
        M5.Lcd.fillRoundRect(posX, posY, side, side, 3, piece.fgColor);
        M5.Lcd.fillRoundRect(posX + 3, posY + 3, side - 6, side - 6, 3, piece.bgColor);
      } else {
        M5.Lcd.fillRoundRect(posX, posY, side, side, 3, piece.bgColor);
      }
    } else {
      uint16_t centerX = posX + side / 2, centerY = posY + side / 2;
      if (_selectedIndex == i) {
        M5.Lcd.fillCircle(centerX, centerY, side / 2, piece.fgColor);
        M5.Lcd.fillCircle(centerX, centerY, side / 2 - 3, piece.bgColor);
      } else {
        M5.Lcd.fillCircle(centerX, centerY, side / 2, piece.bgColor);
      }
    }
    uint16_t wx = M5.Lcd.textWidth(piece.name.c_str());
    M5.Lcd.setTextColor(piece.fgColor, piece.bgColor);
    M5.Lcd.drawString(piece.name.c_str(), posX + (side - wx) / 2, posY + side / 2 - 8, 1);
    posX += side;
    if (i % (_row - 1) == 0 && i > 0) {
      posY += side;
      posX = x + xOffset;
      Serial.println("posX: " + String(posX));
      Serial.println("posY: " + String(posY));
    }
  }
  _inited = true;
}

void M5Mosaic::addValue(MosaicPiece piece) {
  piece.index = _count++;
  _pieces.push_back(piece);
  _inited = false;
}

int8_t M5Mosaic::feedback() {
  if(millis() - _lastCheck < 100) return -1;
  M5.update();
  _lastCheck = millis();
  if (M5.BtnA.wasPressed()) { minusOne(); return -1; }
  if (M5.BtnB.wasPressed()) return _selectedIndex;
  if (M5.BtnC.wasPressed()) { plusOne(); return -1; }
  return -1;
}

void M5Mosaic::minusOne() {
  _selectedIndex -= 1;
  if(_selectedIndex == -1) _selectedIndex = _count;
  draw();
}

void M5Mosaic::plusOne() {
  _selectedIndex += 1;
  if(_selectedIndex == _count) _selectedIndex = 0;
  draw();
}

std::string M5Mosaic::getPieceName(uint8_t ix) {
  return _pieces[ix].name;
}
