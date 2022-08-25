#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "https://energehub-backend-default-rtdb.firebaseio.com";
#define FIREBASE_AUTH "7TIiNuPBBygn1QGffGXBUkuq5SvjSZLqprYFKcJ1";
#define WIFI_SSID "URA";
#define WIFI_PASSWORD "000000d2ad";

#include <Wire.h>
#include <AM2320.h>
AM2320 th;

int dust_sensor = A0;

float dust_value = 0;
float dustDensityug = 0;  // ug/m^3 값을 계산

int sensor_led = D5;      // 미세먼지 센서 안에 있는 적외선 led 핀 번호
int sampling = 280;    // 적외선 led를 키고, 센서 값을 읽어 들여 미세먼지를 측정하는 샘플링 시간
int waiting = 40;    
float stop_time = 9680;   // 센서를 구동하지 않는 시간



float h = 0;
float t = 0;

void setup(){
  Serial.begin(115200);
  Wire.begin();
  pinMode(sensor_led,OUTPUT);
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop(){
  //먼지
  dust_value = 0;
  for(int i = 0; i < 10; i++){
    digitalWrite(sensor_led, LOW);
    delayMicroseconds(sampling);
    dust_value += analogRead(dust_sensor);
//    dust_value = analogRead(dust_sensor);
    delayMicroseconds(waiting);
    digitalWrite(sensor_led, HIGH);
    delayMicroseconds(stop_time);
    delay(100);
  }
//  dust_value = analogRead(dust_sensor);
  Serial.println(dust_value);
  dust_value /= 10;
  Serial.println(dust_value);
  dustDensityug = (0.17 * (dust_value * (3.3 / 1024)) - 0.1) * 1000;
  Serial.print("Dust Density [ug/m3]: ");
  Serial.println(dustDensityug);

  //온습도
  th.Read();
  h = th.h;
  t = th.t;

  //출력
    //먼지
  if(dustDensityug <= 30.0){
    Serial.print("close\n");
  }else if(30.0 < dustDensityug && dustDensityug <= 80.0){
    Serial.print("close\n");
  }else if (80.0 < dustDensityug && dustDensityug <= 150.0){
    Serial.print("open\n");
  }else{
    Serial.print("open\n");
  }
  
    //온습도
  Serial.print(h);
  Serial.print("% \n");
  Serial.print(t);
  Serial.print("℃\n");

  Firebase.pushFloat("dustDnesityug", dustDensityug);
  if (Firebase.failed()) {
      Serial.print("setting /truth failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(100);

  Firebase.pushFloat("humidity", h);
  if (Firebase.failed()) {
      Serial.print("setting /truth failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(100);

  Firebase.pushFloat("temperature", t);
  if (Firebase.failed()) {
      Serial.print("setting /truth failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(4700);

}