#include <WiFi.h>
#include <esp_now.h>
#include <IBusBM.h>
#include <HardwareSerial.h>

// ===== iBUS Setup =====
IBusBM ibus;
HardwareSerial IBusSerial(2);  // UART2 for iBUS input
HardwareSerial IBusOut(1);     // UART1 for iBUS output

const int numChannels = 14;  // Always 14 channels for iBUS output
uint16_t channels[14];       // store channel values

// ===== ESPNOW Data =====
struct ControlData {
  int ch1_offset;  // -100 … +100
  int ch2_offset;  // -100 … +100
};

volatile ControlData remoteData = { 0, 0 };

// ===== ESP-NOW Callback =====
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy((void *)&remoteData, incomingData, sizeof(ControlData));
}

// ===== iBUS Packet Sender =====
void sendIbusFrame() {
  uint8_t packet[32];
  int len = 32;  // fixed 32 bytes for 14 channels

  packet[0] = len;
  packet[1] = 0x40;  // channel data

  // fill channel data
  for (int i = 0; i < numChannels; i++) {
    uint16_t val = channels[i];
    packet[2 + i * 2] = val & 0xFF;
    packet[3 + i * 2] = (val >> 8) & 0xFF;
  }

  // checksum
  uint16_t chksum = 0xFFFF;
  for (int i = 0; i < len - 2; i++) {
    chksum -= packet[i];
  }
  packet[len - 2] = chksum & 0xFF;
  packet[len - 1] = (chksum >> 8) & 0xFF;

  IBusOut.write(packet, len);
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);
  delay(1000);
  digitalWrite(2, 1);

  // ===== iBUS Input (pin 16 RX2) =====
  IBusSerial.begin(115200, SERIAL_8N1, 16, -1);
  ibus.begin(IBusSerial);
  IBusOut.begin(115200, SERIAL_8N1, -1, 17);
  for (int i = 0; i < numChannels; i++) channels[i] = 1500;
  Serial.println("iBUS bridge with ESP-NOW ready...");
}

void loop() {
  static unsigned long lastFrame = 0;

  // update iBUS input
  ibus.loop();
  for (int i = 0; i < 10; i++) {  // FS-iA10B has 10 channels
    channels[i] = ibus.readChannel(i);
  }

  // activation switch is CH5 = index 4
  if (channels[5] >= 1800) {
    channels[0] = constrain(channels[0] + remoteData.ch1_offset, 1000, 2000);
    channels[1] = constrain(channels[1] + remoteData.ch2_offset, 1000, 2000);
  }

  // send at ~7ms interval
  // if (millis() - lastFrame >= 7) {
  //   sendIbusFrame();
  //   lastFrame = millis();
  // }

  // // // debug
  // for (int i = 0; i < 10; i++) {
  //   Serial.print(channels[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();

  sendIbusFrame();
  delay(7);
}
