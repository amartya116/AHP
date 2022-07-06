void setup() {
  pinMode(11, OUTPUT); 
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);// sets the digital pin 13 as output
}

void loop() {
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);// sets the digital pin 13 on
  delay(50000);
  digitalWrite(11,HIGH);
  delay(36000000);// waits for a second
  digitalWrite(12,LOW);
  delay(50000);
}
