#include "BluetoothSerial.h"
BluetoothSerial myserial;
#define pot 35

int store;
int final_value;

void setup() {
  Serial.begin(115200);
  myserial.begin("Omera");
  //Serial.println("Bluetooth Initialized");
}

void loop() {
  
  store = analogRead(35);
  final_value = map(store, 0, 4095, 0, 255);
  Serial.println(final_value);

  delay(300);
  myserial.println(String(final_value));
}