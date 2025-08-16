uint32_t t1, t2 = 0;
int time1 = 10000, time2 = 10000;
#define LatchPin 8;
#define ClockPin 12;
#define DataPin 7;
#define EnablePin 4;
#define l 100
int f;
int interval1 = 10000;
int interval2 = 10000;
unsigned long mls;
bool side = 0;
String Input = "";

void setup() {
  pinMode(LatchPin, OUTPUT);
  pinMode(ClockPin, OUTPUT);
  pinMode(DataPin, OUTPUT);
  pinMode(EnablePin, OUTPUT);
  t1 = t2 = millis();
  Serial.begin(115200);
}

void loop() {
  mls = millis();
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      f = Input.toInt();
      Input = "";
    } else if (c != '\r') {
      Input += c;
    }
  }
  interval1 = time1 - (l * f);
  interval2 = time2 + (l * f);
  interval1 = constrain(interval1, 3000, 15000);
  interval2 = constrain(interval2, 3000, 15000);

  if (!side) {
    if (mls - t1 > (interval1 - 1000)) {
      if (mls - t1 > interval1) {
        side = 1;
        t2 = millis();
        digitalWrite(EnablePin, 1);
        digitalWrite(LatchPin, 0);
        shiftOut(DataPin, ClockPin, 0, 0b00110000);
        digitalWrite(EnablePin, 0);
        digitalWrite(LatchPin, 1);
      } else {
        digitalWrite(EnablePin, 1);
        digitalWrite(LatchPin, 0);
        shiftOut(DataPin, ClockPin, 0, 0b01001000);
        digitalWrite(EnablePin, 0);
        digitalWrite(LatchPin, 1);
      }
    } else {
      digitalWrite(EnablePin, 1);
      digitalWrite(LatchPin, 0);
      shiftOut(DataPin, ClockPin, 0, 0b10000100);
      digitalWrite(EnablePin, 0);
      digitalWrite(LatchPin, 1);
    }
  } else {
    if (mls - t2 > (interval2 - 1000)) {
      if (mls - t2 > interval2) {
        side = 0;
        t1 = millis();
        digitalWrite(EnablePin, 1);
        digitalWrite(LatchPin, 0);
        shiftOut(DataPin, ClockPin, 0, 0b10000100);
        digitalWrite(EnablePin, 0);
        digitalWrite(LatchPin, 1);
      } else {
        digitalWrite(EnablePin, 1);
        digitalWrite(LatchPin, 0);
        shiftOut(DataPin, ClockPin, 0, 0b01001000);
        digitalWrite(EnablePin, 0);
        digitalWrite(LatchPin, 1);
      }
    } else {
      digitalWrite(EnablePin, 1);
      digitalWrite(LatchPin, 0);
      shiftOut(DataPin, ClockPin, 0, 0b00110000);
      digitalWrite(EnablePin, 0);
      digitalWrite(LatchPin, 1);
    }
  }
  Serial.print("side: ");
  Serial.println(side);
  Serial.print("mls - t1: ");
  Serial.println(mls - t1);
  Serial.print("interval1: ");
  Serial.println(interval1);
  Serial.print("mls - t2: ");
  Serial.println(mls - t2);
  Serial.print("interval2: ");
  Serial.println(interval2);
}
