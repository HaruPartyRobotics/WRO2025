int push(byte pin_number) {
  int count = 0;
  if (digitalRead(pin_number) == 0) {
    delay(20);  //debounce delay
    while (digitalRead(pin_number) == 0) delay(20);
    count++;
  }
  return count;
}

int push_count(byte pin_number) {
  int count = 0;
start_point:
  long t = 0;
  if (digitalRead(pin_number) == 0) {
    delay(20);  //debounce delay
    while (digitalRead(pin_number) == 0) {
      delay(20);
      t += 20;
    }
    if (t) {
      count++;
      t = 0;
      while (digitalRead(pin_number) == 1) {  //wait until the button is pressed again
        delay(20);
        t += 20;
        if (t >= 500) return count;
      }
      goto start_point;
    }
  }
  return count;
}