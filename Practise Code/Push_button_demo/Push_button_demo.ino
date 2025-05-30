bool led_state = 0;
void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT_PULLUP);
  pinMode(2, OUTPUT);
}

void loop() {
  int r = push_count(0);
  if (r) {
    Serial.println(r);
    led_state = !led_state;
    digitalWrite(2, led_state);
  }
}
