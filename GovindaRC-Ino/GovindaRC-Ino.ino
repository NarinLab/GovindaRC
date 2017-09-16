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
  
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <Servo.h> 

const char* WIFI_SSID     = "AndroidAP";
const char* WIFI_PASSWORD = "ingattuhan";

#define PIN_PWMA 5
#define PIN_PWMB 4
#define PIN_AIN2 15
#define PIN_AIN1 0
#define PIN_BIN2 2 //D4
#define PIN_BIN1 14 //D5
#define PIN_LIGHT 16
#define PIN_SERVOX 12
#define PIN_SERVOY 13
#define PIN_ECHO 3
#define PIN_TRIG 1
  
#define PAYLOAD_SIZE 256
#define SERVOX_CAL 30 // + if too left, - if too right
#define SERVOY_CAL 30 // + if too up, - if too down
#define ULTRASONIC_MAX 300
#define ULTRASONIC_MIN 3
#define ULTRASONIC_SAMPLE 3

WebSocketsServer webSocket = WebSocketsServer(81, "*", "GovindaRC");
Servo SERVOX;
Servo SERVOY;

// Function Prototype
float scan(int sample = ULTRASONIC_SAMPLE);
//GLOBAL Variable (flag)
int _PWM_A, _PWM_B, _SERVOX, _SERVOY, _DIRECTION;
int _CLIENT_NUM = 0;

// Fail safe
void safeme(){
    // Stop motor
    speed(0,0);
    navigate(0);
    // Led off
    light(0, 0);
    // Watch head
    look(90, 0);
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
            delay(250);
            digitalWrite(PIN_AIN1, LOW);
            digitalWrite(PIN_AIN2, HIGH);
            digitalWrite(PIN_BIN1, LOW);
            digitalWrite(PIN_BIN2, HIGH);
            delay(250);
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
            delay(250);
            digitalWrite(PIN_AIN1, HIGH);
            digitalWrite(PIN_AIN2, LOW);
            digitalWrite(PIN_BIN1, HIGH);
            digitalWrite(PIN_BIN2, LOW);
            delay(250);
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
            delay(250);
            digitalWrite(PIN_AIN1, HIGH);
            digitalWrite(PIN_AIN2, LOW);
            digitalWrite(PIN_BIN1, LOW);
            digitalWrite(PIN_BIN2, HIGH);
            delay(250);
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
            delay(250);
            digitalWrite(PIN_AIN1, LOW);
            digitalWrite(PIN_AIN2, HIGH);
            digitalWrite(PIN_BIN1, HIGH);
            digitalWrite(PIN_BIN2, LOW);
            delay(250);
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

void look(int x, int y) {
    if(x > 180){
        x = 180 - SERVOX_CAL;
    }
    else if(x < 0){
        x = 0 + SERVOX_CAL;
    }
    else{
        x = x - SERVOX_CAL;
    }
    if(y > 180){
        y = 180 - SERVOY_CAL;
    }
    else if(y < 0){
        y = 0  + SERVOY_CAL;
    }
    else{
        y = y - SERVOY_CAL;
    }
    
    SERVOX.write(x);                
    SERVOY.write(y);
    _SERVOX = x;
    _SERVOY = y;
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

void setup() {
  Serial.end();
  //Lighting LED
  pinMode(PIN_LIGHT, OUTPUT);
  //Ultrasonic ranger
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  //Motor A
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  //Motor B
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  //Servo
  pinMode(PIN_SERVOX, OUTPUT);
  pinMode(PIN_SERVOY, OUTPUT);
  SERVOX.attach(PIN_SERVOX);
  SERVOY.attach(PIN_SERVOY);

  //Serial.begin(115200);  
  delay(10);
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  //Serial.println("Trying to connect.");
  while (WiFi.status() != WL_CONNECTED) {
    light(1, 100);
    delay(100);
  }
  //Serial.println("Connected!");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  look(90,90);
}
  
void loop() {
    if(_CLIENT_NUM < 1){
        safeme();
    }
    webSocket.loop();
    if(WiFi.status() != WL_CONNECTED){
        safeme();
    }
}

