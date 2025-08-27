void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myDatar, incomingData, sizeof(myDatar));
  rFlag = 1;
}