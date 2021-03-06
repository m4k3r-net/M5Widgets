#include "M5Bridge.h"

#ifdef L7_
#define LORA_CS_PIN   13
#define LORA_RST_PIN  26
#define LORA_IRQ_PIN  35
#else
#define LORA_CS_PIN   5
#define LORA_RST_PIN  26
#define LORA_IRQ_PIN  36
#endif

// {"from":"kongduino", "msg":"Oh Hai", "sendCount":"1"}

HardwareSerial Serial2 = HardwareSerial(2);
char streamBuffer[256];

void s0Input() {
  if(Serial.available()) {
    Serial.println("In s0Input()");
    uint8_t ix = 0;
    streamBuffer[0] = 0;
    while (Serial.available()) streamBuffer[ix++] = (char)Serial.read();
    streamBuffer[ix] = 0;
    Serial.print("s0: "); Serial.println(streamBuffer);
  }
}

void s2Input() {
  if(Serial2.available()) {
    uint8_t ix = 0;
    streamBuffer[0] = 0;
    Serial.println("In s2Input()");
    while (Serial2.available()) streamBuffer[ix++] = (char)Serial2.read();
    streamBuffer[ix] = 0;
    Serial.print("s2: "); Serial.println(streamBuffer);
  }
}

void loraInput() {
  uint8_t ix = 0;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
  Serial.println("In loraInput()");
    // received a packet
    // print RSSI of packet
    Serial.print(F("Received LoRa packet with RSSI "));
    int rssiLvl = LoRa.packetRssi();
    Serial.println(rssiLvl);
    // read packet
    while (LoRa.available()) {
      char c = LoRa.read();
      streamBuffer[ix++] = c;
    }
    streamBuffer[ix++] = '\0';
  }
}

void s0Output() {
  if (streamBuffer[0] != '\0') {
    Serial.print(streamBuffer);
    Serial.println("In s0Output()");
    streamBuffer[0] = '\0';
  }
}

void s2Output() {
  if (streamBuffer[0] != '\0') {
    Serial2.print(streamBuffer);
    Serial.println("In s2Output()");
    streamBuffer[0] = '\0';
  }
}

void loraOutput() {
  if (streamBuffer[0] != '\0') {
    LoRa.beginPacket();
    LoRa.print(streamBuffer);
    LoRa.endPacket();
    Serial.println("In loraOutput()");
    streamBuffer[0] = '\0';
  }
}

void s0Init(M5Bridge *x) {
  Serial.begin(x->getSerial0Speed());
  Serial.println("Serial inited");
  x->setSerial0Inited(true);
}

void s2Init(M5Bridge *x) {
  Serial2.begin(x->getSerial2Speed());
  Serial.println("Serial2 inited");
  Serial2.println("Serial2 inited");
  x->setSerial2Inited(true);
}

#define REG_MODEM_CONFIG_1 0x1d
#define REG_MODEM_CONFIG_2 0x1e
#define REG_MODEM_CONFIG_3 0x26

void loraInit(M5Bridge *x) {
  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_IRQ_PIN); // set CS, reset, IRQ pin
  Serial.println("LORA_CS_PIN: "+String(LORA_CS_PIN));
  Serial.println("LORA_RST_PIN: "+String(LORA_RST_PIN));
  Serial.println("LORA_IRQ_PIN: "+String(LORA_IRQ_PIN));
  x->initSFBWAGC();
  x->setLoRaInited(true);
  if (!LoRa.begin(x->getLoRaBand())) {
    Serial.println(F("Starting LoRa failed!"));
    x->setLoRaInited(false);
  }
}

M5Bridge::M5Bridge() {
  _serialSourceCount = 0;
  _serialDestinationCount = 0;
  _streamInput0Function = s0Input;
  _streamInput1Function = s2Input;
  _streamOutput0Function = s0Output;
  _streamOutput1Function = s2Output;
  _BAND = 433E6;
  _myLoRaUUID = "DECA-FBAD-M5ST-ACK0";
  _serial0Inited = false, _serial2Inited = false, _loraInited = false;
  _Serial0Speed = 115200;
  _Serial2Speed = 115200;
}

void M5Bridge::initSFBWAGC() {
  Serial.println("SF: 0x" + String(_SF, HEX) + "; BW: 0x" + String(_BW, HEX) + "; AGC: 0x" + String(_AGC, HEX));
  LoRa.writeRegister(REG_MODEM_CONFIG_1, _BW); // 125 kHz, Error coding rate = 4/8, Explicit Header mode
  LoRa.writeRegister(REG_MODEM_CONFIG_2, _SF); // SF = 12 @ 4096 chips / symbol, normal mode, CRC enable, RX Time-Out MSB=0
  LoRa.writeRegister(REG_MODEM_CONFIG_3, _AGC); // Low Data Rate Optimize, AGC enabled
}

void M5Bridge::setLoRaBand(uint32_t myband) {
  _BAND = myband;
}

void M5Bridge::setLoRaUUID(char *myid) {
  _myLoRaUUID = myid;
}

void M5Bridge::setLoRaSFBWAGC(uint8_t sf, uint8_t bw, uint8_t agc) {
  _SF = sf;
  _BW = bw;
  _AGC = agc;
}

void M5Bridge::setIOFunction(uint8_t pipeType0, uint8_t pipeType1) {
  switch (pipeType0) {
    case (tSERIAL0):
      _streamInput0Function = s0Input;
      _streamOutput0Function = s0Output;
      _streamInit0Function = s0Init;
      Serial.println("S0 = Serial");
      break;
    case (tSERIAL2):
      _streamInput0Function = s2Input;
      _streamOutput0Function = s2Output;
      _streamInit0Function = s2Init;
      Serial.println("S0 = Serial2");
      break;
    case (tLoRa):
      _streamInput0Function = loraInput;
      _streamOutput0Function = loraOutput;
      _streamInit0Function = loraInit;
      Serial.println("S0 = LoRa");
      break;
  }
  switch (pipeType1) {
    case (tSERIAL0):
      _streamInput1Function = s0Input;
      _streamOutput1Function = s0Output;
      _streamInit1Function = s0Init;
      Serial.println("S1 = Serial");
      break;
    case (tSERIAL2):
      _streamInput1Function = s2Input;
      _streamOutput1Function = s2Output;
      _streamInit1Function = s2Init;
      Serial.println("S1 = Serial2");
      break;
    case (tLoRa):
      _streamInput1Function = loraInput;
      _streamOutput1Function = loraOutput;
      _streamInit1Function = loraInit;
      Serial.println("S1 = LoRa");
      break;
  }
}

void M5Bridge::start() {
  _isActive = true;
  Serial.println("Calling _streamInit0Function");
  _streamInit0Function(this);
  Serial.println("Calling _streamInit1Function");
  _streamInit1Function(this);
  Serial.println("done");
}

void M5Bridge::run() {
  if (!isActive()) return;
  _streamInput0Function();
  _streamOutput1Function();
  _streamInput1Function();
  _streamOutput0Function();
}

void M5Bridge::stop() {
  _isActive = false;
}

bool M5Bridge::isActive() {
  return _isActive;
}

bool M5Bridge::serial0Inited() {
  return _serial0Inited;
}

bool M5Bridge::serial2Inited() {
  return _serial2Inited;
}

bool M5Bridge::loraInited() {
  return _loraInited;
}

void M5Bridge::setSerial0Inited(bool v) {
  _serial0Inited=v;
}

void M5Bridge::setSerial2Inited(bool v) {
  _serial2Inited=v;
}

void M5Bridge::setLoRaInited(bool v) {
  _loraInited=v;
}

uint32_t M5Bridge::getSerial0Speed(void) {
  return _Serial0Speed;
}

uint32_t M5Bridge::getSerial2Speed(void) {
  return _Serial2Speed;
}

uint32_t M5Bridge::getLoRaBand(void) {
  return _BAND;
}
