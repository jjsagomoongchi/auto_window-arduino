#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

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

//#define VSENSOR A0
#define SWITCH1 D2
int close_status = false;

float vout = 0.0; 
float vin = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;

void setup()
{
  Serial.begin(115200);
  // limit_switch
  pinMode(SWITCH1, INPUT);

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
  
  // Set close fasle (default)
  Serial.printf("Set close false", Firebase.setInt(fbdo, "/product/window/env/close", false) ? "ok" : fbdo.errorReason().c_str());
  pinMode(A0, INPUT);
}
uint64_t lastMillis = 0;
uint64_t nowMillis = 0;

void loop() { //열렸는지 안열였는지 보냄
  nowMillis = millis();
  value = analogRead(A0);
  vout = (value * 5.0) / 1024.0;  //전압값을 계산해주는 공식입니다.
  vin = vout / ( R2 / ( R1 + R2) );

  vin *= 0.6483;
  float vper = (vin - 3.5) * 142.8;

  Serial.print("V: ");
  Serial.println(vin); //현재1.5V 4채널 건전지의 전압값을 출력해줍니다.
  
  if (digitalRead(SWITCH1) == LOW) {
    if (!close_status) {
      Serial.printf("Set close true... %s\n", Firebase.setInt(fbdo, "/product/window/env/close", true) ? "ok" : fbdo.errorReason().c_str());
      close_status = true;
    }
  } else {
    if (close_status) {
      Serial.printf("Set close false", Firebase.setInt(fbdo, "/product/window/env/close", false) ? "ok" : fbdo.errorReason().c_str());
      close_status = false;
    }
  }
  
  if (nowMillis - lastMillis >= 5000 || lastMillis == 0){
    Serial.printf("Set Battery's SOC… %s\n", Firebase.setFloat(fbdo, "/product/window/env/closeV", vin) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set Battery's SOCper… %s\n", Firebase.setFloat(fbdo, "/product/window/batt/closeVper", vper) ? "ok" : fbdo.errorReason().c_str());
    lastMillis = millis();
  }
  delay(500);
}