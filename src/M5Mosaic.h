#include "M5Widget.h"

#ifndef _M5MOSAIC_H_
#define _M5MOSAIC_H_
#define HEADERHEIGHT 30
#define FOOTERHEIGHT 28

#define FF17 &FreeSans9pt7b
#define FF18 &FreeSans12pt7b
#define FF19 &FreeSans18pt7b
#define FF20 &FreeSans24pt7b

#define FF21 &FreeSansBold9pt7b
#define FF22 &FreeSansBold12pt7b
#define FF23 &FreeSansBold18pt7b
#define FF24 &FreeSansBold24pt7b

struct MosaicPiece {
  std::string name;
  uint8_t index;
  uint16_t bgColor;
  uint16_t fgColor;
  bool enabled = true;
  bool square = true;
};

class M5Mosaic : public M5Widget {
 public:
  M5Mosaic(){}
  void addValue(MosaicPiece piece);
  void clear();
  virtual void update(void);
  virtual void draw();
  void setName(String);
  String getName(void);
  std::string getPieceName(uint8_t);
  int8_t feedback(void);
  void plusOne(void);
  void minusOne(void);

 private:
  std::vector <MosaicPiece> _pieces;
  uint8_t _count = 0;
  int8_t _selectedIndex = 0;
  bool _inited;
  uint16_t posX, posY;
  uint8_t side, xOffset, yOffset, nSlots, _row, nLines;
  String _name = "Unnamed";
  uint32_t _lastCheck;
};

#endif
