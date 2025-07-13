#include <esp_now.h>
#include <WiFi.h>

#define l1 4
#define pb 5
#define l2 19
#define l3 16
#define st 14
unsigned long cms = 0;
unsigned long pms = 0;
bool d = 0;

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
  pinMode(l3, OUTPUT);
  pinMode(pb, INPUT_PULLUP);
  pinMode(st, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
  }
  esp_now_register_recv_cb(OnDataRecv);
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    ESP.restart();
  }
}

void loop() {
  myData.b = !digitalRead(pb);
  esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
  delay(100);
  bool rd = digitalRead(st);
  cms = millis();
  if (rd == 0) {
    if (cms - pms >= 500) {
      d = !d;
      digitalWrite(l1, d);
      digitalWrite(l2, !d);
      pms = cms;
    }
  } 
  else{
    digitalWrite(l1, 0);
    digitalWrite(l2, 0);
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myDatar, incomingData, sizeof(myDatar));
    if(myDatar.b){
      digitalWrite(l3, 1);
    }
    else {
      digitalWrite(l3, 0);
    }
  Serial.print("Received Button: ");
  Serial.println(myDatar.b);
  Serial.print("Sent Button: ");
  Serial.println(myData.b);
}
