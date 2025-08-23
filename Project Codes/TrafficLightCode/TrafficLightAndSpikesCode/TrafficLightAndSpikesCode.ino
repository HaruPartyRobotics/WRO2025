#include <ESP32Servo.h>
Servo Servo1, Servo2, Servo3, Servo4;
uint32_t t1, t2 = 0;
int time1 = 5000, time2 = 5000;
#define l 100
#define Servo_Pin1 19
#define Servo_Pin2 22
#define Servo_Pin3 23
#define Servo_Pin4 15
#define LDR_Pin1 33
#define LDR_Pin2 34
#define LDR_Pin3 35
#define LDR_Pin4 36
#define Side_0_Green 13
#define Side_0_Yellow 17
#define Side_0_Red 4
#define Side_1_Green 27
#define Side_1_Yellow 16
#define Side_1_Red 18
#define Limit 0 //Real One Got By Analyzing Real Sensor Values
int f;
int interval1 = 5000;
int interval2 = 5000;
unsigned long mls;
int Flag;
bool side = 0;
String Input = "";

void setup() {
  pinMode(Side_0_Green, OUTPUT);
  pinMode(Side_0_Yellow, OUTPUT);
  pinMode(Side_0_Red, OUTPUT);
  pinMode(Side_1_Green, OUTPUT);
  pinMode(Side_1_Yellow, OUTPUT);
  pinMode(Side_1_Red, OUTPUT);
  Servo1.attach(Servo_Pin1);
  Servo2.attach(Servo_Pin2);
  Servo3.attach(Servo_Pin3);
  Servo4.attach(Servo_Pin4);
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
  int LDR_Reading1 = analogRead(LDR_Pin1);
  int LDR_Reading2 = analogRead(LDR_Pin2);
  int LDR_Reading3 = analogRead(LDR_Pin3);
  int LDR_Reading4 = analogRead(LDR_Pin4);
  if (!side) {
    if (mls - t1 > (interval1 - 1000)) {
      if (mls - t1 > interval1) {
        side = 1;
        t2 = millis();
        Flag = 1;
        digitalWrite(Side_0_Green, 0);
        digitalWrite(Side_0_Yellow, 0);
        digitalWrite(Side_0_Red, 1);
        digitalWrite(Side_1_Green, 1);
        digitalWrite(Side_1_Yellow, 0);
        digitalWrite(Side_1_Red, 0);
      } else {
        Flag = 2;
        digitalWrite(Side_0_Green, 0);
        digitalWrite(Side_0_Yellow, 1);
        digitalWrite(Side_0_Red,0);
        digitalWrite(Side_1_Green, 0);
        digitalWrite(Side_1_Yellow, 1);
        digitalWrite(Side_1_Red, 0);
      }
    } else {
      Flag = 3;
        digitalWrite(Side_0_Green, 1);
        digitalWrite(Side_0_Yellow, 0);
        digitalWrite(Side_0_Red, 0);
        digitalWrite(Side_1_Green, 0);
        digitalWrite(Side_1_Yellow, 0);
        digitalWrite(Side_1_Red, 1);
    }
  } else {
    if (mls - t2 > (interval2 - 1000)) {
      if (mls - t2 > interval2) {
        side = 0;
        Flag = 3;
        t1 = millis();
        digitalWrite(Side_0_Green, 1);
        digitalWrite(Side_0_Yellow, 0);
        digitalWrite(Side_0_Red, 0);
        digitalWrite(Side_1_Green, 0);
        digitalWrite(Side_1_Yellow, 0);
        digitalWrite(Side_1_Red, 1);
      } else {
        Flag = 2;
        digitalWrite(Side_0_Green, 0);
        digitalWrite(Side_0_Yellow, 1);
        digitalWrite(Side_0_Red, 0);
        digitalWrite(Side_1_Green, 0);
        digitalWrite(Side_1_Yellow, 1);
        digitalWrite(Side_1_Red, 0);
      }
    } else {
        Flag = 1;
        digitalWrite(Side_0_Green, 0);
        digitalWrite(Side_0_Yellow, 0);
        digitalWrite(Side_0_Red, 1);
        digitalWrite(Side_1_Green, 1);
        digitalWrite(Side_1_Yellow, 0);
        digitalWrite(Side_1_Red, 0);
    }
  }
  if(LDR_Reading1 < Limit && Flag == 1){
    Servo1.write(0);
  }
  else if(LDR_Reading2 < Limit && Flag == 3){
    Servo2.write(0);
  }
  else if(LDR_Reading3 < Limit && Flag == 1){
    Servo3.write(0);
  }
  else if(LDR_Reading4 < Limit && Flag == 3){
    Servo4.write(0);
  }
  else{
    Servo1.write(90);
    Servo2.write(90);
    Servo3.write(90);
    Servo4.write(90);
  }
  Serial.print("Interval 1: ");
  Serial.println(interval1);
  Serial.print("Side 0: ");
  Serial.println(mls - t1);
  Serial.print("Interval 2: ");
  Serial.println(interval2);
  Serial.println("Side 1: ");
  Serial.println(mls - t2);
  delay(100);
}
