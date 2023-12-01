/* 
 * Project Light Fixture City of ABQ
 * Author: Emily Silva
 * Date: 01-DEC-2023
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"
#include "math.h"
#include "Colors.h"
#include "Grove-Ultrasonic-Ranger.h"
#include <Adafruit_MQTT.h>
#include "credentials.h"
#include <Adafruit_MQTT/Adafruit_MQTT_SPARK.h>
#include <Adafruit_MQTT/Adafruit_MQTT.h>

const int sigPin = A5;
const float FREQ = 0.2;
const int PIXELCOUNT = 60;
const int LIGHTCOLOR = 0xFF8B17; // Hex color for 2700K light
const int publishTime = 600000;
Adafruit_NeoPixel pixel (PIXELCOUNT, SPI, WS2812B);

float t;
float scale[PIXELCOUNT];
int i;
int cred, cgreen, cblue;
int ledPin1 = D1;
int ledPin2 = D2;
int ledPin3 = D3;
int ledPin4 = D4;
int ledPin5 = D5;
int ledPin6 = D6;
int distanceInch;
float distance;
float inch;
int rangeInInches;

Ultrasonic ultrasonic(sigPin);
SYSTEM_MODE(SEMI_AUTOMATIC);

/************ Global State (you don't need to change this!) ***   ***************/ 
TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Subscribe buttonFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buttononoff"); 
Adafruit_MQTT_Publish randomFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/randomnumber");

/************Declare Variables*************/
unsigned int last, lastTime;
float subValue,pubValue;

/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();
float buttonOnOff;

SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup() {
  Serial.begin(9600);
 // waitFor(Serial.isConnected,15000);

// Connect to Internet but not Particle Cloud
   //WiFi.on();
   //WiFi.connect();
   //while(WiFi.connecting()) {
   //Serial.printf(".");

  // }

  //Serial.printf("\n\n");

// Setup MQTT subscription
  //mqtt.subscribe(&subFeed); 

  pinMode (ledPin1, OUTPUT);
  pinMode (ledPin2, OUTPUT);
  pinMode (ledPin3, OUTPUT);
  pinMode (ledPin4, OUTPUT);
  pinMode (ledPin5, OUTPUT);
  pinMode (ledPin6, OUTPUT);


  pixel.begin();
  pixel.setBrightness(255);
  pixel.show();

  cred = LIGHTCOLOR >> 16;
  cgreen = LIGHTCOLOR >> 8 & 0xFF;
  cblue = LIGHTCOLOR & 0xFF;
}


void loop() {
  rangeInInches = ultrasonic.MeasureInInches();
  Serial.printf("Distance:%i\n", rangeInInches);

  if ((rangeInInches<5)&&(rangeInInches>0)){
   digitalWrite (ledPin1, 255);
  //  delay (5000);
  //  digitalWrite (ledPin1, 192);
  //  delay (5000);
  //  digitalWrite (ledPin1, 129);
  //  delay (5000);
  //  digitalWrite (ledPin1, 63);
  //  delay (5000);
  //  digitalWrite (ledPin1, LOW);
  
}

if ((rangeInInches<7)&&(rangeInInches>5)){
   digitalWrite (ledPin2, 255);
  //  delay (5000);
  //  digitalWrite (ledPin2, 192);
  //  delay (5000);
  //  digitalWrite (ledPin2, 129);
  //  delay (5000);
  //  digitalWrite (ledPin2, 63);
  //  delay (5000);
    digitalWrite (ledPin2, LOW);
  
}

if ((rangeInInches<9)&&(rangeInInches>7)){
   digitalWrite (ledPin3, 255);
  //  delay (5000);
  //  digitalWrite (ledPin3, 192);
  //  delay (5000);
  //  digitalWrite (ledPin3, 129);
  //  delay (5000);
  //  digitalWrite (ledPin3, 63);
  //  delay (5000);
    digitalWrite (ledPin3, LOW);
  
}

if ((rangeInInches<11)&&(rangeInInches>9)){
   digitalWrite (ledPin4, 255);
  //  delay (5000);
  //  digitalWrite (ledPin4, 192);
  //  delay (5000);
  //  digitalWrite (ledPin4, 129);
  //  delay (5000);
  //  digitalWrite (ledPin4, 63);
  //  delay (5000);
    digitalWrite (ledPin4, LOW);
  
}

if ((rangeInInches<13)&&(rangeInInches>11)){
   digitalWrite (ledPin5, 255);
  //  delay (5000);
  //  digitalWrite (ledPin5, 192);
  //  delay (5000);
  //  digitalWrite (ledPin5, 129);
  //  delay (5000);
  //  digitalWrite (ledPin5, 63);
  //  delay (5000);
    digitalWrite (ledPin5, LOW);
}

if ((rangeInInches<15)&&(rangeInInches>13)){
   digitalWrite (ledPin6, 255);
  //  delay (5000);
  //  digitalWrite (ledPin6, 192);
  //  delay (5000);
  //  digitalWrite (ledPin6, 129);
  //  delay (5000);
  //  digitalWrite (ledPin6, 63);
  //  delay (5000);
    digitalWrite (ledPin6, LOW);
  
}

    t = millis()/1000.0;

  for(i=0;i<PIXELCOUNT;i++) {
    
    scale[i] = 0.25*sin(2*M_PI*((FREQ*t) - (i/(float)PIXELCOUNT)))+0.3;
    pixel.setPixelColor(i,scale[i]*cred,scale[i]*cgreen,scale[i]*cblue);
  }
  pixel.show();
}