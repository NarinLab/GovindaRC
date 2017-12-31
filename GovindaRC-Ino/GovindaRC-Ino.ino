//MIT License
//
//Copyright (c) 2017 Narin Laboratory
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include <EEPROM.h>  
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>

const char* DEFAULT_WIFI_SSID     = "AndroidAP";
const char* DEFAULT_WIFI_PASSWORD = "ingattuhan";
char WIFI_SSID[32] = "";
char WIFI_PASSWORD[32] = "";

#define PIN_PWMA 5
#define PIN_PWMB 4
#define PIN_AIN2 15
#define PIN_AIN1 0
#define PIN_BIN2 2 //D4
#define PIN_BIN1 14 //D5
#define PIN_LIGHT 16
#define PIN_ENGINE 12
#define PIN_ECHO 3
#define PIN_TRIG 1
  
#define PAYLOAD_SIZE 256
#define ULTRASONIC_MAX 300
#define ULTRASONIC_MIN 3
#define ULTRASONIC_SAMPLE 3

WebSocketsServer webSocket = WebSocketsServer(81, "*", "GovindaRC");

// Function Prototype
float scan(int sample = ULTRASONIC_SAMPLE);
//GLOBAL Variable (flag)
int _PWM_A, _PWM_B, _DIRECTION;
int _CLIENT_NUM = 0;
bool _FLAG_SCAN = false;
uint16_t _FLAG_SCAN_PERIOD = 100; //millisecond
unsigned long _FLAG_SCAN_LAST = 0;


/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, WIFI_SSID);
  EEPROM.get(0+sizeof(WIFI_SSID), WIFI_PASSWORD);
  char ok[2+1];
  EEPROM.get(0+sizeof(WIFI_SSID)+sizeof(WIFI_PASSWORD), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    String(DEFAULT_WIFI_SSID).toCharArray(WIFI_SSID, sizeof(WIFI_SSID) - 1);
    String(DEFAULT_WIFI_PASSWORD).toCharArray(WIFI_PASSWORD, sizeof(WIFI_PASSWORD) - 1);
    saveCredentials();
  }
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, WIFI_SSID);
  EEPROM.put(0+sizeof(WIFI_SSID), WIFI_PASSWORD);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(WIFI_SSID)+sizeof(WIFI_PASSWORD), ok);
  EEPROM.commit();
  EEPROM.end();
  ESP.reset();
}


// Fail safe
void safeme(){
    // Stop motor
    engine_switch(0);
    speed(0,0);
    navigate(0);
    // Led off
    light(0, 0);
}

void engine_switch(int state){
    digitalWrite(PIN_ENGINE, state);
}

void speed(int pwm_a, int pwm_b){
    if(pwm_a > 1023){
        pwm_a = 1023;
    }
    else if(pwm_a < 0){
        pwm_a = 0;
    }
    if(pwm_b > 1023){
        pwm_b = 1023;
    }
    else if(pwm_b < 0){
        pwm_b = 0;
    }
    _PWM_A = pwm_a;
    _PWM_B = pwm_b;
    analogWrite(PIN_PWMA, pwm_a);
    analogWrite(PIN_PWMB, pwm_b);
}

void navigate(int direction) {
    // 0 =  stop, 1 = forward, 2 = backward, 3 = left, 4 = rights
    if ( direction == 1 ) {
        if(_DIRECTION != 1 && _PWM_A > 256 && _PWM_B > 256){
            // Loosing gas
            int pwm_a = _PWM_A;
            int pwm_b = _PWM_B;
            speed(256,256);
            //delay(250);
            digitalWrite(PIN_AIN1, LOW);
            digitalWrite(PIN_AIN2, HIGH);
            digitalWrite(PIN_BIN1, LOW);
            digitalWrite(PIN_BIN2, HIGH);
            //delay(250);
            // Restoring gas
            speed(pwm_a, pwm_b);
        }
        else{
            digitalWrite(PIN_AIN1, LOW);
            digitalWrite(PIN_AIN2, HIGH);
            digitalWrite(PIN_BIN1, LOW);
            digitalWrite(PIN_BIN2, HIGH);
        }

        _DIRECTION = 1;
    }
    else if ( direction == 2 ) {
        if(_DIRECTION != 2 && _PWM_A > 256 && _PWM_B > 256){
            // Loosing gas
            int pwm_a = _PWM_A;
            int pwm_b = _PWM_B;
            speed(256,256);
            //delay(250);
            digitalWrite(PIN_AIN1, HIGH);
            digitalWrite(PIN_AIN2, LOW);
            digitalWrite(PIN_BIN1, HIGH);
            digitalWrite(PIN_BIN2, LOW);
            //delay(250);
            // Restoring gas
            speed(pwm_a, pwm_b);
        }
        else{
            digitalWrite(PIN_AIN1, HIGH);
            digitalWrite(PIN_AIN2, LOW);
            digitalWrite(PIN_BIN1, HIGH);
            digitalWrite(PIN_BIN2, LOW);
        }

        _DIRECTION = 2;
    }
    else if ( direction == 3 ) {
        if(_DIRECTION != 3 && _PWM_A > 256 && _PWM_B > 256){
            // Loosing gas
            int pwm_a = _PWM_A;
            int pwm_b = _PWM_B;
            speed(256,256);
            //delay(250);
            digitalWrite(PIN_AIN1, HIGH);
            digitalWrite(PIN_AIN2, LOW);
            digitalWrite(PIN_BIN1, LOW);
            digitalWrite(PIN_BIN2, HIGH);
            //delay(250);
            // Restoring gas
            speed(pwm_a, pwm_b);
        }
        else{
            digitalWrite(PIN_AIN1, HIGH);
            digitalWrite(PIN_AIN2, LOW);
            digitalWrite(PIN_BIN1, LOW);
            digitalWrite(PIN_BIN2, HIGH);
        }

        _DIRECTION = 3;
    }
    else if ( direction == 4 ) {
        if(_DIRECTION != 4 && _PWM_A > 256 && _PWM_B > 256){
            // Loosing gas
            int pwm_a = _PWM_A;
            int pwm_b = _PWM_B;
            speed(256,256);
            //delay(250);
            digitalWrite(PIN_AIN1, LOW);
            digitalWrite(PIN_AIN2, HIGH);
            digitalWrite(PIN_BIN1, HIGH);
            digitalWrite(PIN_BIN2, LOW);
            //delay(250);
            // Restoring gas
            speed(pwm_a, pwm_b);
        }
        else{
            digitalWrite(PIN_AIN1, LOW);
            digitalWrite(PIN_AIN2, HIGH);
            digitalWrite(PIN_BIN1, HIGH);
            digitalWrite(PIN_BIN2, LOW);
        }

        _DIRECTION = 4;
    }
    else {
        digitalWrite(PIN_AIN1, HIGH);
        digitalWrite(PIN_AIN2, HIGH);
        digitalWrite(PIN_BIN1, HIGH);
        digitalWrite(PIN_BIN2, HIGH);

        _DIRECTION = 0;
    }
}

void light(int state, int hold){
    if(hold > 0){
        digitalWrite(PIN_LIGHT, !state);
        delay(hold);
        digitalWrite(PIN_LIGHT, state);
    }
    else{
        digitalWrite(PIN_LIGHT, !state);
    }
}

float scan(int sample){
    float cms = 0;
    
    for(int i = 0; i < sample; i++) {
        digitalWrite(PIN_TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(PIN_TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(PIN_TRIG, LOW);
        long duration = pulseIn(PIN_ECHO, HIGH);
        int cm = duration / 29 / 2;
        if(cm > 300){
            cm = ULTRASONIC_MAX;
        }
        else if(cm < 3){
            cm = ULTRASONIC_MIN;
        }
        cms += cm;
        delay(5);
    }
    
    return cms / sample;
}

void stream_ultrasonic(){
    if(_FLAG_SCAN){
        StaticJsonBuffer<PAYLOAD_SIZE> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        char payload[PAYLOAD_SIZE];
        float cm = scan(5);
        root["msgId"] = "stream";
        root["status"] = 200;
        root["type"] = "scan";
        root["data"] = cm;
        root["desc"] = "OK";
        root.printTo(payload, sizeof(payload));
        webSocket.broadcastTXT(payload);
    }
}

void setup_ota(){
    ArduinoOTA.begin();
    ArduinoOTA.onStart([]() {
      
    });
  
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      
    });
  
    ArduinoOTA.onEnd([]() {
      
    });
}

void setup() {
  Serial.end();
  //Serial.begin(115200);  
  //delay(10);
  //Serial.println("Serial ready!");
  

  loadCredentials();
  
  //Lighting LED
  pinMode(PIN_LIGHT, OUTPUT);
  //Ultrasonic ranger (Disable if using Serial)
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  //Engine starter
  pinMode(PIN_ENGINE, OUTPUT);
  //Motor A
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  //Motor B
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  //Serial.println("Trying to connect.");
  while (WiFi.status() != WL_CONNECTED) {
    light(1, 100);
    delay(100);
  }
  //Serial.println("Connected!");
  setup_ota();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
  
void loop() {
    ArduinoOTA.handle();
    /*webSocket.loop();*/
    /* SCHEDULER */
    unsigned long NOW = millis();
    if((NOW - _FLAG_SCAN_LAST) > _FLAG_SCAN_PERIOD){
        stream_ultrasonic();
        _FLAG_SCAN_LAST = NOW;
    }
    /* SCHEDULER */
    if(_CLIENT_NUM < 1){
        safeme();
    }
    if(WiFi.status() != WL_CONNECTED){
        safeme();
    }
}

