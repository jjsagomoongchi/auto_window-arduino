int SWITCH1 = 3;

void setup() {
  pinMode(SWITCH1, INPUT);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(SWITCH1) == LOW){
      Serial.println("SWITCH1");
  }
}
