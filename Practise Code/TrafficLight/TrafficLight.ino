uint32_t t1, t2 = 0;
int time1 = 10000, time2 = 10000;
int interval1;
int interval2;
#define side0green 4
#define side0yellow 12
#define side0red 13
#define side1green 16
#define side1yellow 17
#define side1red 27
#define l 100
unsigned long mls;
bool side = 0;
void setup() {
  pinMode(4, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(27, OUTPUT);
  t1 = t2 = millis();
  Serial.begin(115200);
}

void loop() {
  mls = millis();
  if (Serial.available()) {
    int f = Serial.parseInt();
    interval1 = time1 - l * f;
    interval2 = time2 + l * f;
    interval1 = constrain(interval1, 3000, 15000);
    interval2 = constrain(interval2, 3000, 15000);
  }
  if (!side) {
    if (mls - t1 > interval1) {
      side = 1;
      digitalWrite(side0green, 0);
      digitalWrite(side0yellow, 0);
      digitalWrite(side0red, 1);
      digitalWrite(side1green, 1);
      digitalWrite(side1yellow, 0);
      digitalWrite(side1red, 0);
      t2 = millis();
    }
    else if (mls - t1 > (interval1 - 1000)) {
      digitalWrite(side0green, 0);
      digitalWrite(side0yellow, 1);
      digitalWrite(side0red, 0);
      digitalWrite(side1green, 0);
      digitalWrite(side1yellow, 1);
      digitalWrite(side1red, 0);
    }
    else {
      digitalWrite(side0green, 1);
      digitalWrite(side0yellow, 0);
      digitalWrite(side0red, 0);
      digitalWrite(side1green, 0);
      digitalWrite(side1yellow, 0);
      digitalWrite(side1red, 1);
    }
  } else {
    if (mls - t2 > interval2) {
      side = 0;
      digitalWrite(side0green, 1);
      digitalWrite(side0yellow, 0);
      digitalWrite(side0red, 0);
      digitalWrite(side1green, 0);
      digitalWrite(side1yellow, 0);
      digitalWrite(side1red, 1);
      t1 = millis();
    }
    else if (mls - t2 > (interval2 - 1000)) {
      digitalWrite(side0green, 0);
      digitalWrite(side0yellow, 1);
      digitalWrite(side0red, 0);
      digitalWrite(side1green, 0);
      digitalWrite(side1yellow, 1);
      digitalWrite(side1red, 0);
    }
    else {
      digitalWrite(side0green, 0);
      digitalWrite(side0yellow, 0);
      digitalWrite(side0red, 1);
      digitalWrite(side1green, 1);
      digitalWrite(side1yellow, 0);
      digitalWrite(side1red, 0);
    }
  }
}
