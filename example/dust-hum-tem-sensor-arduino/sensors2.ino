
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP8266
 *
 * Copyright (c) 2022 mobizt
 *
 */

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

//센서
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


/* 1. Define the WiFi credentials */
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

/* 2. Define the API Key */
#define API_KEY "api_key"

/* 3. Define the RTDB URL */
#define DATABASE_URL "##.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "emailThatHaveAuth"
#define USER_PASSWORD "emailPassword"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;



void setup()
{

  Serial.begin(115200);
  Wire.begin();
  pinMode(sensor_led, OUTPUT);
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

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

}

void loop()
{
  dust_value = 0;
  for(int i = 0; i < 10; i++){
    digitalWrite(sensor_led, LOW);
    delayMicroseconds(sampling);
    dust_value += analogRead(dust_sensor);
//    dust_value = analogRead(dust_sensor);
    delayMicroseconds(waiting);
    digitalWrite(sensor_led, HIGH);
    delayMicroseconds(stop_time);
    delay(500);
  }
//  dust_value = analogRead(dust_sensor);
  Serial.print("dust sum ");
  Serial.println(dust_value);
  dust_value /= 10;
  Serial.print("dust once");
  Serial.println(dust_value);
  dustDensityug = (0.17 * (dust_value * (5.0 / 1024)) - 0.1) * 1000;
  Serial.print("Dust Density [ug/m3]: ");
  Serial.println(dustDensityug);

  //온습도
  th.Read();
  h = th.h;
  t = th.t;

  Serial.print(h);
  Serial.println("%");
  Serial.print(t);
  Serial.println("℃");
  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    Serial.printf("Set dust... %s\n", Firebase.setFloat(fbdo, "/test/dust", dustDensityug) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set hum... %s\n", Firebase.setFloat(fbdo, "/test/hum", h) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set tem... %s\n", Firebase.setFloat(fbdo, "/test/tem", t) ? "ok" : fbdo.errorReason().c_str());
  }
}