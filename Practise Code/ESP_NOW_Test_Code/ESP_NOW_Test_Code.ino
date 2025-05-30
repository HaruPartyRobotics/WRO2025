#include <esp_now.h>
#include <WiFi.h>


uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
bool message = 0;
int send = 1;
bool status=1;
int but_status;

esp_now_peer_info_t peerInfo;
esp_err_t result;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  pinMode(0, INPUT_PULLUP);
  pinMode(2, OUTPUT);
}

void loop() {
  if(message){
    message =0;
    digitalWrite(2,but_status);
  }
  
  status = !digitalRead(0);
  if(send!=status){
    send = status;
    esp_now_send(broadcastAddress,(uint8_t*)&send, sizeof(send));
  }

}


void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&but_status, incomingData, sizeof(but_status));
  message = 1;
}