#include "BluetoothSerial.h"
BluetoothSerial myserial;
#define pot 35
#define led 4
int inpwm;
int frequency = 1000;
int channel = 0;
int final_value;

void setup() {
  Serial.begin(115200);
  myserial.begin("Master", true);
  ledcSetup(0, 1000, 8);
  ledcAttachPin(4, 0);

  if (myserial.connect("Omera")) {
    Serial.println("Bluetooth Initialized");
  } else {
    Serial.println("Not Connected. Restart your ESP32 by pressing enable button.");
  }
}

void loop() {

  if (myserial.available()>0) {
    String pwm = myserial.readString();
    intpwm = pwm.toInt();
    Serial.println(intpwm);

    ledcWrite(0, intpwm);
  }
}