int red_0 = 5;
int yellow_0 = 4;
int green_0 = 3;

int red_1 = 6;
int yellow_1 = 7;
int green_1 = 8;

unsigned long x = 0;
int time1 = 8000;
int time2 = 8000;
bool dir = 0;

void setup() {
  pinMode(red_0, OUTPUT);
  pinMode(yellow_0, OUTPUT);
  pinMode(green_0, OUTPUT);
  pinMode(red_1, OUTPUT);
  pinMode(yellow_1, OUTPUT);
  pinMode(green_1, OUTPUT);

  Serial.begin(9600);

  x = millis();
}

void loop() {
  if (dir == 0) {
    if ((millis() - x) > time1) {
      dir = 1;
      digitalWrite(red_0, 1;
      digitalWrite(green_1, 1);
    } 
    else if ((millis() - x) > (time1 - 1000)) {
      digitalWrite(yellow_0, 1);
      digitalWrite(yellow_1, 1);
    }
  }

  if (Serial.available()) {
    String sig = Serial.readStringUntil(';');
    int signal = sig.toInt();   //Convert

    Serial.print("Signal Given: ");
    Serial.println(signal);

  }
}