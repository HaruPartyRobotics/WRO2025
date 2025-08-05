
#define ledr 4
#define ledy 5
#define ledg 19

void setup() {
  pinMode(ledr,OUTPUT);
  pinMode(ledy,OUTPUT);
  pinMode(ledg,OUTPUT);

}

void loop() {
  digitalWrite(ledr,HIGH);
  digitalWrite(ledy,LOW);
  digitalWrite(ledg,LOW);
  delay(2000);
  digitalWrite(ledr,LOW);
  digitalWrite(ledy,HIGH);
  digitalWrite(ledg,LOW);
  delay(2000);
  digitalWrite(ledr,LOW);
  digitalWrite(ledy,LOW);
  digitalWrite(ledg,HIGH);
  delay(2000);
}  
