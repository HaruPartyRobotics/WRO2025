#include <esp_now.h>
#include <WiFi.h>

#define buzzerPin 13
#define pbPin 5
#define nlight 4
#define emergencyLedPin 19
#define MAX_NORMAL 12
#define MAX_EMERGENCY_LIFETIME 20
#define BUZZER_CHANNEL 0

int normalCarIDs[MAX_NORMAL];
bool normalCarActive[MAX_NORMAL];
int normalCount = 0;
uint8_t emergencyIdsLifetime[MAX_EMERGENCY_LIFETIME];
bool emergencyIdsBuzzerDone[MAX_EMERGENCY_LIFETIME];
bool emergencyLedActive[MAX_EMERGENCY_LIFETIME];
int emergencyIdsCount = 0;

typedef struct struct_message {
  uint8_t id;
  bool b;
} struct_message;

struct_message myData;
struct_message myDatar;

uint8_t peerAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
esp_now_peer_info_t peerInfo;

bool lastButtonState = false;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pbPin, INPUT_PULLUP);
  pinMode(emergencyLedPin, OUTPUT);
  digitalWrite(emergencyLedPin, LOW);
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
  ledcSetup(BUZZER_CHANNEL, 2000, 8);
  ledcAttachPin(buzzerPin, BUZZER_CHANNEL);
}

void loop() {
  bool buttonPressed = !digitalRead(pbPin);
  myData.id = 1;
  myData.b = buttonPressed;
  esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
  delay(100);
  if(myDatar.b){
    if(myDatar.id < 90){
      playBuzzerTone(normalCount);
    }
  }
  else{
    ledcWriteTone(BUZZER_CHANNEL, 0);
  }
  Serial.print("Index is, ");
  Serial.println(normalCount);
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myDatar, incomingData, sizeof(myDatar));
  uint8_t rcvId = myDatar.id;
  bool rcvB = myDatar.b;

  if (rcvId >= 1 && rcvId <= 12) {
    if (rcvB) {
      addOrActivateNormal(rcvId);
    } else {
      removeNormal(rcvId);
    }
  } else if (rcvId >= 90 && rcvId <= 92) {
    handleEmergencySignal(rcvId, rcvB);
  }
  updateEmergencyLed();
}

void addOrActivateNormal(int id) {
  for (int i = 0; i < normalCount; i++) {
    if (normalCarIDs[i] == id) {
      normalCarActive[i] = true;
      return;
    }
  }
  if (normalCount < MAX_NORMAL) {
    normalCarIDs[normalCount] = id;
    normalCarActive[normalCount] = true;
    normalCount++;
  }
}

void removeNormal(int id) {
  for (int i = 0; i < normalCount; i++) {
    if (normalCarIDs[i] == id) {
      for (int j = i; j < normalCount - 1; j++) {
        normalCarIDs[j] = normalCarIDs[j + 1];
        normalCarActive[j] = normalCarActive[j + 1];
      }
      normalCount--;
      break;
    }
  }
}

void handleEmergencySignal(uint8_t rcvId, bool rcvB) {
  int idx = -1;
  for (int i = 0; i < emergencyIdsCount; i++) {
    if (emergencyIdsLifetime[i] == rcvId) {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    if (emergencyIdsCount < MAX_EMERGENCY_LIFETIME) {
      emergencyIdsLifetime[emergencyIdsCount] = rcvId;
      emergencyIdsBuzzerDone[emergencyIdsCount] = false;
      emergencyLedActive[emergencyIdsCount] = false;
      idx = emergencyIdsCount;
      emergencyIdsCount++;
    } else {
      return;
    }
  }
  if (rcvB) {
    if (!emergencyIdsBuzzerDone[idx]) {
      playSpecialBuzzer();
      emergencyIdsBuzzerDone[idx] = true;
      emergencyLedActive[idx] = false;
    } else {
      if (!emergencyLedActive[idx]) {
        emergencyLedActive[idx] = true;
      }
    }
  } else {
    if (emergencyLedActive[idx]) {
      emergencyLedActive[idx] = false;
    }
  }
}

void updateEmergencyLed() {
  bool anyActive = false;
  for (int i = 0; i < emergencyIdsCount; i++) {
    if (emergencyLedActive[i]) {
      anyActive = true;
      break;
    }
  }
  digitalWrite(emergencyLedPin, anyActive ? HIGH : LOW);
}

void playBuzzerTone(int count) {
  int freq = 280 + count * 16;
  ledcWriteTone(BUZZER_CHANNEL, freq);
}

void playSpecialBuzzer() {
  ledcWriteTone(BUZZER_CHANNEL, 1000);
  delay(300);
  ledcWriteTone(BUZZER_CHANNEL, 0);
}
