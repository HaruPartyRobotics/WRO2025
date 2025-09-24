#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

typedef struct struct_message {
  uint8_t id;
  bool b;
} struct_message;

bool emergency = 0;

#define l 100
#define Servo_Pin1 13
#define Servo_Pin2 12
#define Servo_Pin3 14
#define Servo_Pin4 27
#define servo_Pin1 25
#define servo_Pin2 33
#define servo_Pin3 14
#define servo_Pin4 32
struct_message myDatar;
Servo Servo1, Servo2, Servo3, Servo4, servo1, servo2, servo3, servo4;
uint32_t mls, updt, mls2;
int time1 = 5000, time2 = 5000;
int led[6] = { 5, 18, 19, 4, 2, 15 };
int interval1 = time1;
int interval2 = time2;
int curr, f, dsp = 0;
int road;
bool side, srv1, srv2, srv3, srv4 = 0;
String Input = "";
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
  }
  esp_now_register_recv_cb(OnDataRecv);
  for (byte i = 0; i < 6; i++) pinMode(led[i], OUTPUT);
  Servo1.attach(Servo_Pin1);
  Servo2.attach(Servo_Pin2);
  Servo3.attach(Servo_Pin3);
  Servo4.attach(Servo_Pin4);
  servo1.attach(servo_Pin1);
  servo2.attach(servo_Pin2);
  servo3.attach(servo_Pin3);
  servo4.attach(servo_Pin4);
  Servo1.write(90);
  Servo2.write(90);
  Servo3.write(0);
  Servo4.write(0);
  mls = millis();
  (!side) ? curr = interval1 + mls - millis() : curr = interval2 + mls - millis();
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    int values[5];
    int index = 0;
    char *ptr = strtok((char*)line.c_str(), ",");
    while (ptr != NULL && index < 5) {
      values[index++] = atoi(ptr);
      ptr = strtok(NULL, ",");
    }
    if (index == 5) {
      dsp = values[0];
      srv1 = values[1];
      srv2 = values[2];
      srv3 = values[3];
      srv4 = values[4];
    }
  }

  (!side) ? curr = interval1 + mls - millis() : curr = interval2 + mls - millis();

  servo1.write(90*(!srv1 == 0));
  servo2.write(90*(!srv2 == 0));
  servo3.write(90*(!srv3 == 0));
  servo4.write(90*(!srv4 == 0));

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
    digitalWrite(led[1], 0);
    digitalWrite(led[4], 0);
    signal_update();
    road = (!side) ? 1 : 2;
    mls = millis();
  }

  else if (curr < 1000) {
    for (byte i = 0; i < 6; i++) digitalWrite(led[i], 0);
    digitalWrite(led[1], 1);
    digitalWrite(led[4], 1);
  }

  if (millis() - updt >= 10) {
    Serial.print(emergency);
    Serial.print(",");
    Serial.print(side);
    Serial.print(",");
    Serial.print(curr);
    Serial.print(",");
    Serial.println(road);
    updt = millis();
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myDatar, incomingData, sizeof(myDatar));
  if (myDatar.id >= 90 && myDatar.id <= 99) emergency = myDatar.b;
}

void signal_update() {
  interval1 = time1 - (l * f);
  interval2 = time2 + (l * f);
  interval1 = constrain(interval1, 3000, 15000);
  interval2 = constrain(interval2, 3000, 15000);
}
