#define l1 4
#define l2 19
#define pb 5
#define bz 13
int ct = 0;
void setup() {
  Serial.begin(115200);
  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
  pinMode(pb, INPUT_PULLUP);
  pinMode(bz, OUTPUT);
  digitalWrite(l1, 0);
  digitalWrite(l2, 0);
  digitalWrite(bz, 0);
}

void loop() {
  bool rd = digitalRead(pb);
  if (rd == 0) {
    ct++;
    delay(200);
  }
  if (ct == 1) {
    digitalWrite(l1, 1);
    digitalWrite(l2, 0);
    digitalWrite(bz, 0);
    Serial.println("Turning On Red LED! 🔴 ");
  } else if (ct == 2) {
    digitalWrite(l2, 1);
    digitalWrite(l1, 0);
    digitalWrite(bz, 0);
    Serial.println("Turning On Blue LED! 🔵 ");
  } else if (ct == 3) {
    digitalWrite(bz, 1);
    digitalWrite(l1, 0);
    digitalWrite(l2, 0);
    Serial.println("Turning On Buzzer! 📯  ");
  }
  else if(ct == 4){
  digitalWrite(l1, 0);
  digitalWrite(l2, 0);
  digitalWrite(bz, 0);
  Serial.println("Stopping ⛔ ");
  }
  else if(ct > 4){
    ct = 1;
  }
}
