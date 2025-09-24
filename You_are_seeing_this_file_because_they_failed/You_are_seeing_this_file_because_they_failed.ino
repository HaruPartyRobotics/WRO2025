#include <esp_now.h>
#include <WiFi.h>

#define vehicle_ID 4

#define pbPin 0
#define buzzerPin 27
#define nlight 26
#define amb_button 25
#define BUZZER_CHANNEL 0
unsigned long presstime, releasetime, newpress, buzzerdelay, oldmls, pressed_timer = 0, amb_interval;
bool laststate = 1;
bool state, buzzerstate = 0;
int duration = 0;

int vehicle_index[2][10] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                             { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
int vehicle_sum = 0;

typedef struct struct_message {
  uint8_t id;
  bool b;
} struct_message;

struct_message myData;
struct_message vehicle;

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
esp_now_peer_info_t peerInfo;

bool lastButtonState = false, rFlag = 0, lastHornState = 0, button_pressed = 0, long_pressed = 0, ambulance = 0;


void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pbPin, INPUT_PULLUP);
  pinMode(amb_button, INPUT_PULLUP);
  pinMode(nlight, OUTPUT);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
  }
  esp_now_register_recv_cb(OnDataRecv);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    ESP.restart();
  }
  ledcSetup(BUZZER_CHANNEL, 1000, 8);
  ledcAttachPin(buzzerPin, BUZZER_CHANNEL);
  ledcWriteTone(BUZZER_CHANNEL, 0);
  for (byte i = 0; i < 3; i++) {
    digitalWrite(nlight, 1);
    delay(100);
    digitalWrite(nlight, 0);
    delay(100);
  }

  newpress = millis();
}

void loop() {
  if (millis() - newpress > 20) {
    state = !digitalRead(pbPin);
    ambulance = !digitalRead(amb_button);
    newpress = millis();
  }

  if (state && !button_pressed) {
    button_pressed = 1;
    myData.id = vehicle_ID;
    myData.b = 1;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    Serial.println("Sent Data 1");
    presstime = millis();
  }

  if (state && !long_pressed && millis() - presstime > 1000) {
    ledcWriteTone(BUZZER_CHANNEL, 3000);
    Serial.println("Long Pressed!");
    long_pressed = 1;
  }

  else if (!state && button_pressed) {
    button_pressed = 0;
    if (long_pressed) {
      ledcWriteTone(BUZZER_CHANNEL, 0);
      long_pressed = 0;
    }
    myData.id = vehicle_ID;
    myData.b = 0;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    Serial.println("Sent Data 0");
  }

  if (rFlag) {
    rFlag = 0;
    Serial.printf("Car ID: %d -- Horn State: %d\n", vehicle.id, vehicle.b);

    if (vehicle.id < 90) {
      indexing();
      for (byte i = 0; i < 10; i++) Serial.printf("%d-", vehicle_index[0][i]);
      Serial.println();
      for (byte i = 0; i < 10; i++) Serial.printf("%d-", vehicle_index[1][i]);
      Serial.println();
      ledcWriteTone(BUZZER_CHANNEL, 2000 * (vehicle_sum > 0) + 200 * vehicle_sum);
    }

    else if (vehicle.b == 1) {
      ledcWriteTone(BUZZER_CHANNEL, 3000);
      digitalWrite(nlight, 1);
      delay(100);
      ledcWriteTone(BUZZER_CHANNEL, 0);
      digitalWrite(nlight, 0);
      delay(100);
      ledcWriteTone(BUZZER_CHANNEL, 3000);
      digitalWrite(nlight, 1);
      delay(100);
      ledcWriteTone(BUZZER_CHANNEL, 0);
      digitalWrite(nlight, 0);
      delay(100);
    }
  }

  if (millis() - amb_interval > 1000 && ambulance) {
    myData.id = 91;
    myData.b = 1;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    amb_interval = millis();
  }
}
