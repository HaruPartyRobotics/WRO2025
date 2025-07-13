#include <esp_now.h>
#include <WiFi.h>

#define l1 4
#define pb 5
#define l2 19

bool g = 0;
bool fg = 0;
int ct = 0;
unsigned long pms = 0;

uint8_t peerAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

typedef struct struct_message {
  bool b;
} struct_message;

struct_message myData;
struct_message myDatar;
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
  pinMode(pb, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    ESP.restart();
  }

  esp_now_register_recv_cb(OnDataRecv);
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer Add Failed");
    ESP.restart();
  }
}

void loop() {
  myData.b = !digitalRead(pb);
  esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
  delay(100);
  if (fg) {
    unsigned long cms = millis();
    if (cms - pms >= 500) {
      g = !g;
      digitalWrite(l1, g);
      digitalWrite(l2, !g);
      pms = cms;
      ct++;
      if (ct >= 7) {
        fg = false;
        ct = 0;
        digitalWrite(l1, 0);
        digitalWrite(l2, 0);
      }
    }
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myDatar, incomingData, sizeof(myDatar));
  if (myDatar.b) {
    fg = true;
    ct = 0;
    digitalWrite(l1, 0);
    digitalWrite(l2, 1);
    pms = millis();
  }
  Serial.print("Received button: ");
  Serial.println(myDatar.b);
  Serial.print("Sent Button: ");
  Serial.println(myData.b);
}