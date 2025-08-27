#include <esp_now.h>
#include <WiFi.h>

#define buzzerPin 13
#define hornPin 12
#define pbPin 23
#define nlight 4
#define emergencyLedPin 19
#define BUZZER_CHANNEL 0

typedef struct struct_message {
  uint8_t id;
  bool b;
} struct_message;

struct_message myData;
struct_message myDatar;

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
esp_now_peer_info_t peerInfo;

bool lastButtonState = false, rFlag = 0;


void setup() {
  pinMode(pbPin, INPUT_PULLUP);
  pinMode(hornPin, INPUT_PULLUP);
  pinMode(nlight, OUTPUT);
  pinMode(emergencyLedPin, OUTPUT);
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
  ledcSetup(BUZZER_CHANNEL, 2000, 8);
  ledcAttachPin(buzzerPin, BUZZER_CHANNEL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
