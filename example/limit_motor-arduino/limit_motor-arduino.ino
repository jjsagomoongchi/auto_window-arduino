int motorB1 = 2;
int motorB2 = 3;
int SWITCH1 = 4;

void setup() {
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(SWITCH1, INPUT);
}

void loop() {
  // 앞으로 회전
  if(digitalRead(SWITCH1) == LOW){
    analogWrite(motorB1,0);
    analogWrite(motorB2,0);
  } else {
    analogWrite(motorB1,255);
    analogWrite(motorB2,0);
  }
//  delay(1000);
//    
//    
//    // 뒤로 회전
//  analogWrite(motorB1,0);
//  analogWrite(motorB2,255);
//  delay(1000);
//    
//    // 정지
//  analogWrite(motorB1,0);
//  analogWrite(motorB2,0);
//  delay(1000);
}
