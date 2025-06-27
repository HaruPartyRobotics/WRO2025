#include <esp_now.h>
#include <WiFi.h>

int l = 4;
#define pb 5
#define deviceid 1

uint8_t peerAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

typedef struct struct_message {
  bool b;
  int i;
} struct_message;

struct_message myData;
struct_message myDatar;
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  pinMode(l, OUTPUT);
  pinMode(pb, INPUT_PULLUP);
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
  digitalWrite(l, 0);
}

void loop() {
  myData.b = !digitalRead(pb);
  myData.i = deviceid;
  esp_now_send(peerAddress, (uint8_t )&myData, sizeof(myData));
  delay(100);
}

void OnDataRecv(const uint8_tmac, const uint8_t *incomingData, int len) {
  memcpy(&myDatar, incomingData, sizeof(myDatar));
  if (myDatar.i != deviceid) {
    if(myDatar.b){
      digitalWrite(l, 1);
    }
        else if(myDatar.b == 0){
      digitalWrite(l, 0);
    }
    Serial.print("Received button: ");
    Serial.println(myDatar.b);
  }
}
