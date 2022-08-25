#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
//#include <FirebaseESP8266.h>
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

unsigned long sendDataPrevMillis = 0;
unsigned long sendDataPrevMillis2 = 0;

int isMotorOn = 0;
int isOpen = 0;

int motorB1 = 2;
int motorB2 = 0;

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

  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
}

void loop() {
  if(Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    int isOpen = 0;
    int isClose = 0;

    Firebase.getInt(fbdo, F("/product/test/window/env/open"), &isOpen);
    Firebase.getInt(fbdo, F("/product/test/window/env/close"), &isClose);

    if(!isOpen) {
      int dust = 0;
      Firebase.getInt(fbdo, F("/product/test/window/env/dust"), &dust);

      if(dust > 1) {
        isMotorOn = 1;
        Serial.println("dust is bigger than 2, but not openned , motor on");
      }

      if(isMotorOn) {
        analogWrite(motorB1,0);
        analogWrite(motorB2, 255);
      }
    }

    if(isOpen) {
      Serial.println("openned, motor off");
      isMotorOn = 0;
      analogWrite(motorB1,0);
      analogWrite(motorB2, 0);
    }
  }
}