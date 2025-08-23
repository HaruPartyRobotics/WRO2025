#include <ESP32Servo.h>
Servo Servo1, Servo2, Servo3, Servo4;
uint32_t mls, updt;
int time1 = 5000, time2 = 5000;
#define l 100
#define Servo_Pin1 19
#define Servo_Pin2 22
#define Servo_Pin3 23
#define Servo_Pin4 15
int led[6] = { 13, 17, 4, 27, 16, 18 };
int interval1 = time1;
int interval2 = time2;
int curr, f;
bool side = 0;
String Input = "";

void setup() {
  for (byte i = 0; i < 6; i++) pinMode(led[i], OUTPUT);
  Servo1.attach(Servo_Pin1);
  Servo2.attach(Servo_Pin2);
  Servo3.attach(Servo_Pin3);
  Servo4.attach(Servo_Pin4);
  Servo1.write(90);
  Servo2.write(90);
  Servo3.write(0);
  Servo4.write(0);
  Serial.begin(115200);
  mls = millis();
  (!side) ? curr = interval1 + mls - millis() : curr = interval2 + mls - millis();
}

void loop() {
  while (Serial.available()) {
    Input = Serial.readStringUntil(10);
    f = Input.toInt();
    Serial.println(f);
  }

  (!side) ? curr = interval1 + mls - millis() : curr = interval2 + mls - millis();

  if (curr <= 0) {
    side = !side;
    Servo1.write(90 * (side == 0));
    Servo2.write(90 * (side == 0));
    Servo3.write(90 * (side == 1));
    Servo4.write(90 * (side == 1));
    digitalWrite(led[0], (side == 0));
    digitalWrite(led[5], (side == 0));
    digitalWrite(led[2], (side == 1));
    digitalWrite(led[3], (side == 1));
    signal_update();
    mls = millis();  //timer reset
  }

  else if (curr < 1000) {  //for yellow light activation
    for (byte i = 0; i < 6; i++) digitalWrite(led[i], 0);
    digitalWrite(led[1], 0);
    digitalWrite(led[4], 0);
  }

  if (millis() - updt > 100) {
    Serial.print("Interval 1: ");
    Serial.print(interval1);
    Serial.print("  Interval 2: ");
    Serial.print(interval2);
    Serial.print("  Side: ");
    Serial.print(side);
    Serial.print("  Current: ");
    Serial.println(curr);
    updt = millis();
  }
}

void signal_update() {
  interval1 = time1 - (l * f);
  interval2 = time2 + (l * f);
  interval1 = constrain(interval1, 3000, 15000);
  interval2 = constrain(interval2, 3000, 15000);
}
