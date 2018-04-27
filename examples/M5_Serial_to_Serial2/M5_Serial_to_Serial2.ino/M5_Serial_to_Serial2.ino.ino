
HardwareSerial ss = HardwareSerial(2);
void setup() {
  Serial.begin(115200);
  ss.begin(9600);
  Serial.println("\nSoftware serial test started");
}

void loop() {
  while (ss.available() > 0) {
    Serial.write(ss.read());
  }
  while (Serial.available() > 0) {
    ss.write(Serial.read());
  }

}
