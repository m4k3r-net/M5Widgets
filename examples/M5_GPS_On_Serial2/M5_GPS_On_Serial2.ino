HardwareSerial gpsSerial = HardwareSerial(2);
char buffer[256];
int count = 0;
// counter for buffer array
bool hasFix = false;
float myLat, myLong;
String myLatText, myLongText, fixTime, fixDate;

String getdms(double ang, bool isLat = true) {
  bool neg(false);
  if (ang < 0.0) {
    neg = true;
    ang = -ang;
  }
  int deg = (int)ang;
  double frac = ang - (double)deg;
  frac *= 60.0;
  int min = (int)frac;
  frac = frac - (double)min;
  double sec = nearbyint(frac * 600000.0);
  sec /= 10000.0;
  if (sec >= 60.0) {
    min++;
    sec -= 60.0;
  }
  String oss;
  if (neg) oss = "-";
  oss += String(deg) + "d " + String(min) + "' " + String(sec) + "\"";
  if (isLat) {
    if (neg) oss += "S";
    else oss += "N";
  } else {
    if (neg) oss += "W";
    else oss += "E";
  }
  return oss;
}

void setup() {
  delay(2000);
  // the gpsSerial baud rate
  gpsSerial.begin(9600);
  // the Serial port of Arduino baud rate.
  Serial.begin(115200);
  Serial.println("\n\n+-----------------------+");
  Serial.println("+      GPS Tester       +");
  Serial.println("+-----------------------+");
}

void loadUpToDollar() {
  char c = gpsSerial.read();
  while (c != '$') {
    c = gpsSerial.read();
  }
}

int skipToNext(char *bf, char x, int pos) {
  int ix = pos;
  while (bf[ix] != x) {
    ix += 1;
  }
  return ix;
}

void loop() {
  if (gpsSerial.available()) {
    loadUpToDollar();
    while (gpsSerial.available()) {
      // reading data into char array
      char c = gpsSerial.read();
      buffer[count++] = c; // write data into array
      if (count == 128)break;
      if (c == 10)break;
      if (c == 13)break;
    }
    buffer[count] = 0;
    //if (count > 5) Serial.println(buffer);
    if (strncmp (buffer, "GPRMC", 5) == 0 || strncmp (buffer, "GNRMC", 5) == 0) {
      Serial.write("\n$");
      Serial.println(buffer);
      if (strncmp (buffer, "GPRMC,,", 7) == 0 || strncmp (buffer, "GNRMC,,", 7) == 0) goto TheEnd;
      // Invalid
      int ix = skipToNext(buffer, ',', 0) + 1;
      fixTime = String(buffer[ix++]) + String(buffer[ix++]) + ":" + String(buffer[ix++]) + String(buffer[ix++]) + ":" + String(buffer[ix++]) + String(buffer[ix++]) + " UTC";
      ix = skipToNext(buffer, ',', ix) + 1;
      char c = buffer[ix++];
      if (c != 'A') {
        hasFix = false;
      } else {
        ix++;
        Serial.println("  [ok]");
        // Valid
        int yx = skipToNext(buffer, '.', ix);
        int i;
        String s = "";
        for (i = ix; i < yx; i++) s = s + String(buffer[i]);
        int Latitude = s.toInt();
        int Lat1 = Latitude / 100;
        Serial.print("Latitude: ");
        int Lat1b = (Latitude - (Lat1 * 100));
        s = "";
        yx += 1;
        ix = skipToNext(buffer, ',', yx);
        for (i = yx; i < ix; i++)s = s + String(buffer[i]);
        i += 1;
        c = buffer[i];
        i += 2;
        double Lat2 = s.toInt();
        Lat2 = Lat1b + (Lat2 / 100000);
        myLat = Lat1 + Lat2 / 100;
        myLatText = getdms(myLat, true);
        Serial.println(myLatText);
        // Longitude
        Serial.print("Longitude: ");
        yx = skipToNext(buffer, '.', i);
        s = "";
        for (ix = i; ix < yx; ix++) s = s + String(buffer[ix]);
        int Lontitude = s.toInt();
        int Lont1 = Lontitude / 100;
        int Lont1b = (Lontitude - (Lont1 * 100));
        s = "";
        yx = ix + 1;
        ix = skipToNext(buffer, ',', yx);
        for (i = yx; i < ix; i++) s = s + String(buffer[i]);
        i += 1;
        c = buffer[i];
        i += 2;
        double Lont2 = s.toInt();
        Lont2 = Lont1b + (Lont2 / 100000);
        myLong = Lont1 + Lont2 / 100;
        myLongText = getdms(myLong, false);
        Serial.println(myLongText);
        ix = skipToNext(buffer, ',', yx);
        yx = ix + 1;
        ix = skipToNext(buffer, ',', yx);
        yx = ix + 1;
        ix = skipToNext(buffer, ',', yx);
        yx = ix + 1;
        ix = skipToNext(buffer, ',', yx);
        yx = ix + 1;
        fixDate="20"+String(buffer[yx+4])+String(buffer[yx+5])+"/"+String(buffer[yx+2])+String(buffer[yx+3])+"/"+String(buffer[yx])+String(buffer[yx+1]);
        Serial.println("Fix taken at: "+fixTime+" on "+fixDate);
        hasFix = true;
      }
    }
TheEnd:
    clearBufferArray(); // call clearBufferArray function to clear the storaged data from the array
    count = 0; // set counter of while loop to zero
  }
  if (Serial.available())
    // if data is available on hardwareserial port ==> data is coming from PC or notebook
    gpsSerial.write(Serial.read()); // write it to the gpsSerial shield
}

void clearBufferArray() {
  // function to clear buffer array
  for (int i = 0; i < count; i++) {
    buffer[i] = NULL;
  } // clear all index of array with command NULL
}
