void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&vehicle, incomingData, sizeof(vehicle));
  rFlag = 1;
}