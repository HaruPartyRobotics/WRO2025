#include <esp_now.h>
#include <WiFi.h>

#define buzzerPin 13
#define hornPin 12
#define pbPin 23
#define nlight 15
#define emergencyLedPin 19
#define BUZZER_CHANNEL 0

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

bool lastButtonState = false, rFlag = 0, lastHornState = 0;


void setup() {
  myData.id = 6;
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
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
  bool pbState = !digitalRead(pbPin);
  bool hState = !digitalRead(hornPin);

  if (hState != lastHornState) {
    lastHornState = hState;
    if (hState) ledcWriteTone(BUZZER_CHANNEL, 5000);
    else ledcWriteTone(BUZZER_CHANNEL, 500 * vehicle_sum);
    Serial.printf("Front Horn: %s\n", (hState) ? "ON" : "OFF");
  }


  if (pbState != lastButtonState) {
    lastButtonState = pbState;
    myData.b = pbState;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    Serial.printf("Silent Horn: %s\n", (pbState) ? "ON" : "OFF");
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
      if (!hState) ledcWriteTone(BUZZER_CHANNEL, 500 * vehicle_sum);
    }

    else {
      if (vehicle.b == 1) {
        digitalWrite(nlight, 1);
        ledcWriteTone(BUZZER_CHANNEL, 5000);
        delay(50);
        ledcWriteTone(BUZZER_CHANNEL, 0);
        delay(50);
        ledcWriteTone(BUZZER_CHANNEL, 5000);
        delay(50);
        ledcWriteTone(BUZZER_CHANNEL, 0);
        digitalWrite(nlight, 0);
        vehicle.b = 0;
      }
    }
  }
}
