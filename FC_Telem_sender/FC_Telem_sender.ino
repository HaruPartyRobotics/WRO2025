#include <WiFi.h>
#include <esp_now.h>

// ===== Data Structure =====
struct ControlData {
  int ch1_offset;
  int ch2_offset;
};

ControlData dataToSend = {0, 0};
ControlData lastData = {999, 999};  // store last sent (init to impossible values)

// ===== Receiver MAC Address =====
// Replace with your receiver ESP32’s MAC
uint8_t receiverMac[] = {0x6C, 0xC8, 0x40, 0x93, 0x60, 0x90};

// ===== Send Callback =====
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// ===== Helper: Parse input string "X,Y;" =====
bool parseSerialInput(ControlData &outData) {
  static String buffer = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == ';') {
      // parse full packet
      int commaIndex = buffer.indexOf(',');
      if (commaIndex > 0) {
        String xStr = buffer.substring(0, commaIndex);
        String yStr = buffer.substring(commaIndex + 1);
        outData.ch1_offset = constrain(xStr.toInt(), -100, 100);
        outData.ch2_offset = constrain(yStr.toInt(), -100, 100);
        buffer = "";
        return true;
      }
      buffer = ""; // reset if malformed
    } 
    
    else {
      buffer += c;
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200);

  // WiFi + ESP-NOW Init
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW ❌");
    return;
  }
  esp_now_register_send_cb(onDataSent);

  // Add peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer ❌");
    return;
  }

  Serial.println("Sender ready ✅");
  Serial.println("Enter offsets as: X,Y;  (example: 50,-30;)");
}

void loop() {
  // Try to read serial input
  if (parseSerialInput(dataToSend)) {
    // Only send if changed
    if (dataToSend.ch1_offset != lastData.ch1_offset ||
        dataToSend.ch2_offset != lastData.ch2_offset) {
      
      esp_now_send(receiverMac, (uint8_t*)&dataToSend, sizeof(dataToSend));
      Serial.printf("Sent: CH1=%d, CH2=%d\n", dataToSend.ch1_offset, dataToSend.ch2_offset);

      lastData = dataToSend; // update last sent
    }
  }
}
