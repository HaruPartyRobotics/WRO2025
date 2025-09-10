
int R_1    = 4;
int Y_1 = 7;
int G_1  = 8;

int r_2   = 3;
int y_2= 5;
int g_2  =6;

unsigned long lastSwitchTime = 0;
const int G_DURATION  = 8000;  
const int Y_DURATION = 1000;  


bool dir = false;

void setup() {
  pinMode(R_1, OUTPUT);
  pinMode(Y_1, OUTPUT);
  pinMode(G_1, OUTPUT);
  pinMode(r_2, OUTPUT);
  pinMode(y_2, OUTPUT);
  pinMode(g_2, OUTPUT);

  Serial.begin(9600);
  lastSwitchTime = millis();

  
  setLightsForDirection(dir);
}

void loop() {
  unsigned long elapsed = millis() - lastSwitchTime;

  
  if (elapsed >= (G_DURATION - Y_DURATION) && elapsed < G_DURATION) {
    setYellowLights();
  }
 
  else if (elapsed >= G_DURATION) {
    dir = !dir; 
    setLightsForDirection(dir);
    lastSwitchTime = millis();
  }

 
  if (Serial.available()) {
    String sig = Serial.readStringUntil(';');
    int signal = sig.toInt();
    Serial.print("Signal Given: ");
    Serial.println(signal);
  }
}


void setLightsForDirection(bool direction) {
  if (direction == false) {
    digitalWrite(G_1, HIGH);
    digitalWrite(R_1, LOW);
    digitalWrite(g_2, LOW);
    digitalWrite(r_2, HIGH);
  } else {
    digitalWrite(G_1, LOW);
    digitalWrite(R_1, HIGH);
    digitalWrite(g_2, HIGH);
    digitalWrite(r_2, LOW);
  }
  digitalWrite(Y_1, LOW);
  digitalWrite(y_2, LOW);
}


void setYellowLights() {
  digitalWrite(Y_1, HIGH);
  digitalWrite(y_2, HIGH);
  digitalWrite(G_1, LOW);
  digitalWrite(g_2, LOW);
}
