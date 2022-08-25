#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

float vout = 0.0;
float vin = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"
#define API_KEY "API_KEY"
#define DATABASE_URL "https://DATABASE_NAME.firebaseio.com"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int Motor = 0;
int isOpen = 0;
int isClose = 0;

#define Vsensor A0
#define motorO D1
#define motorC D2

void setup()
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
  pinMode(Vsensor, INPUT);
  pinMode(motorO, OUTPUT);
  pinMode(motorC, OUTPUT);
  digitalWrite(motorO, LOW);
  digitalWrite(motorC, LOW);
}

void loop() {
  value = analogRead(Vsensor);
  vout = (value * 5.0) / 1024.0;  //전압값을 계산해주는 공식입니다.
  vin = vout / ( R2 / ( R1 + R2) );

  vin *= 0.6564;

  Serial.print("V: ");
  Serial.println(vin); //현재1.5V 4채널 건전지의 전압값을 출력해줍니다.
  Firebase.setFloat(fbdo, F("/product/window/env/motorV"), vin);
  Firebase.getInt(fbdo, F("/product/window/env/motor"), &Motor);
  Serial.print("---");
  Serial.println(Motor);
  switch (Motor) { //모터 돌릴지멈출지 뒤로갈지
    case 1: //열기
      Firebase.getInt(fbdo, F("/product/window/env/open"), &isOpen);
      while (!isOpen && Motor == 1) {
        digitalWrite(motorO, HIGH);
        digitalWrite(motorC, LOW);
        delay(1000);
        Firebase.getInt(fbdo, F("/product/window/env/open"), &isOpen);
        Firebase.getInt(fbdo, F("/product/window/env/motor"), &Motor);
        Serial.print("-");
        Serial.println(isOpen);
      }
      digitalWrite(motorO, LOW);
      Serial.println("open done");
      if (Motor == 1) {
        Firebase.setInt(fbdo, F("/product/window/env/motor"), 0);
      }
      break;
    case 2: //닫기
      Firebase.getInt(fbdo, F("/product/window/env/close"), &isClose);
      while (!isClose && Motor == 2) {
        digitalWrite(motorO, LOW);
        digitalWrite(motorC, HIGH);
        delay(1000);
        Firebase.getInt(fbdo, F("/product/window/env/close"), &isClose);
        Firebase.getInt(fbdo, F("/product/window/env/motor"), &Motor);
        Serial.print("- -");
        Serial.println(isClose);
      }
      digitalWrite(motorC, LOW);
      Serial.println("close done");
      if (Motor == 2) {
        Firebase.setInt(fbdo, F("/product/window/env/motor"), 0);
      }
      break;
    default:
      digitalWrite(motorO, LOW);
      digitalWrite(motorC, LOW);
      Serial.println("nothing");
      break;
  }

  delay(1000);
  Serial.println("ye");
}