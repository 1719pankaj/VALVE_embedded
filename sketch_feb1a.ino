// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "time.h"
#include <EEPROM.h>
#define EEPROM_SIZE 32

#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "J_WICK"
#define WIFI_PASSWORD "bitemebitemebiteme"

#define API_KEY "AIzaSyC7XSxBJN6AbzC5kvFtNyFaR3GBekBHW_0"
#define DATABASE_URL "https://espautovalve-default-rtdb.asia-southeast1.firebasedatabase.app" 

#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <RtcDS1307.h>
RtcDS1307<TwoWire> Rtc(Wire);

RTC_DS1307 DS1307_RTC;

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int TRASH_VALVE = 4;
int FRESH_VALVE = 2;

int trashOpenTimeHr = 0;
int trashOpenTimeMi = 0;
int trashCloseTimeHr = 0;
int trashCloseTimeMi = 45;

int freshOpenTimeHr = 0;
int freshOpenTimeMi = 45;
int freshCloseTimeHr = 1;
int freshCloseTimeMi = 15;

// int trashOpenTimeHr = 15;
// int trashOpenTimeMi = 1;
// int trashCloseTimeHr = 15;
// int trashCloseTimeMi = 3;

// int freshOpenTimeHr = 15;
// int freshOpenTimeMi = 3;
// int freshCloseTimeHr = 15;
// int freshCloseTimeMi = 5;

const char* ntpServer = "in.pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;//GMT+5:30

unsigned long Epoch_Time; 

unsigned long Get_Epoch_Time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}


void setup () {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  int wifiTol = 0;
  while (WiFi.status() != WL_CONNECTED){
    if(wifiTol > 3) {
      inCaseOfFire();
      break;      
    }
    Serial.print(".");
    delay(300);
    wifiTol++;
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.println("Setting up NTP Time");
    configTime(gmtOffset_sec, 0, ntpServer);

    Rtc.Begin();

    time_t unixTimeStamp = Get_Epoch_Time(); //Jan 1, 2014 00:00:00
    RTC.set(unixTimeStamp + gmtOffset_sec);
    Serial.println("Internet Time set");
    Serial.println("DS1307RTC Read Test");
    Serial.println("-------------------");
    

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    /* Sign up */
    if (Firebase.signUp(&config, &auth, "", "")){
      Serial.println("ok");
      signupOK = true;
    }
    else{
      Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    config.token_status_callback = tokenStatusCallback;
    
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

  //====================================================================

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/trashOpenTimeHr")) {
        if (fbdo.dataType() == "int") {
          trashOpenTimeHr = fbdo.intData();
          EEPROM.write(0, trashOpenTimeHr);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/trashOpenTimeMi")) {
        if (fbdo.dataType() == "int") {
          trashOpenTimeMi = fbdo.intData();
          EEPROM.write(4, trashOpenTimeMi);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/trashCloseTimeHr")) {
        if (fbdo.dataType() == "int") {
          trashCloseTimeHr = fbdo.intData();
          EEPROM.write(8, trashCloseTimeHr);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/trashCloseTimeMi")) {
        if (fbdo.dataType() == "int") {
          trashCloseTimeMi = fbdo.intData();
          EEPROM.write(12, trashCloseTimeMi);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/freshOpenTimeHr")) {
        if (fbdo.dataType() == "int") {
          freshOpenTimeHr = fbdo.intData();
          EEPROM.write(16, freshOpenTimeHr);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/freshOpenTimeMi")) {
        if (fbdo.dataType() == "int") {
          freshOpenTimeMi = fbdo.intData();
          EEPROM.write(20, freshOpenTimeMi);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/freshCloseTimeHr")) {
        if (fbdo.dataType() == "int") {
          freshCloseTimeHr = fbdo.intData();
          EEPROM.write(24, freshCloseTimeHr);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }

    if (Firebase.ready() && signupOK ){
      if (Firebase.RTDB.getInt(&fbdo, "/test/freshCloseTimeMi")) {
        if (fbdo.dataType() == "int") {
          freshCloseTimeMi = fbdo.intData();
          EEPROM.write(28, freshCloseTimeMi);
          EEPROM.commit();
          // Serial.println(intValue);
        }
      } else {
        Serial.println(fbdo.errorReason());
        inCaseOfFire();
      }
    }      
  }  

//====================================================================

  pinMode(TRASH_VALVE, OUTPUT);
  pinMode(FRESH_VALVE, OUTPUT);

// #ifndef ESP8266
//   while (!Serial); // wait for serial port to connect. Needed for native USB
// #endif

  if (!DS1307_RTC.begin()) {
    Serial.println("Couldn't find RTC");
    while(1);
  }
  //  DS1307_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop () {
    DateTime now = DS1307_RTC.now();

    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    // Serial.println(now.hour());
    // Serial.println(trashOpenTimeHr);
    // Serial.println(now.hour() >= trashOpenTimeHr);
    // Serial.println(now.minute() >=  trashOpenTimeMi);
    // Serial.println(now.minute());
    // Serial.println(trashOpenTimeMi);    
    // Serial.println(now.hour() <= trashCloseTimeHr);
    // Serial.println(now.minute() <= trashCloseTimeMi);    

    if(now.hour() >= trashOpenTimeHr && now.minute() >=  trashOpenTimeMi && now.hour() <= trashCloseTimeHr && now.minute() <= trashCloseTimeMi) {
      digitalWrite(TRASH_VALVE, HIGH);
      digitalWrite(FRESH_VALVE, LOW);

      Serial.print("Bleeder On, Fresh off");
      Serial.println();
    } else if(now.hour() >= freshOpenTimeHr && now.minute() >=  freshOpenTimeMi && now.hour() <= freshCloseTimeHr && now.minute() <= freshCloseTimeMi) {
      digitalWrite(FRESH_VALVE, HIGH);
      digitalWrite(TRASH_VALVE, LOW);
      Serial.print("Fresh On, Bleeder off");
      Serial.println();
    } else {
      digitalWrite(FRESH_VALVE, LOW);
      digitalWrite(TRASH_VALVE, LOW);
      Serial.print("Waiting...");
      Serial.println();
    }

    Serial.println();
    Serial.println();
    delay(3000);
}

int inCaseOfFire() {

  Serial.println("Fire Fire!");
  trashOpenTimeHr = EEPROM.read(0);
  trashOpenTimeMi = EEPROM.read(4);
  trashCloseTimeHr = EEPROM.read(8);
  trashCloseTimeMi = EEPROM.read(12);

  freshOpenTimeHr = EEPROM.read(16);
  freshOpenTimeMi = EEPROM.read(20);
  freshCloseTimeHr = EEPROM.read(24);
  freshCloseTimeMi = EEPROM.read(28);

  return 0;
}
