/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"
#include "math.h"
#include "Colors.h"
#include "Adafruit_SSD1306.h"
#include "Grove-Ultrasonic-Ranger.h"

const int sigPin = D16;
const float FREQ = 0.2;
const int PIXELCOUNT = 60;
const int LIGHTCOLOR = 0xFF8B17; // Hex color for 2700K light
Adafruit_NeoPixel pixel (PIXELCOUNT, SPI, WS2812B);

float t;
float scale[PIXELCOUNT];
int i;
int cred, cgreen, cblue;
int ledPin1 = D6;
int ledPin2 = D5;
int ledPin3 = D4;
int ledPin4 = D3;
int ledPin5 = D2;
int ledPin6 = D1;
long rangeInInches;

Ultrasonic ultrasonic(sigPin);
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
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

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  rangeInInches = ultrasonic.MeasureInInches();
  Serial.printf("Distance:%d\n", rangeInInches);

  if ((rangeInInches<5)&&(rangeInInches>0)){
   digitalWrite (ledPin1, HIGH);
   delay (30000);
   digitalWrite (ledPin1, LOW);
  
}

if ((rangeInInches<7)&&(rangeInInches>5)){
   digitalWrite (ledPin2, HIGH);
   delay (30000);
   digitalWrite (ledPin2, LOW);
  
}

if ((rangeInInches<9)&&(rangeInInches>7)){
   digitalWrite (ledPin3, HIGH);
   delay (30000);
   digitalWrite (ledPin3, LOW);
  
}

if ((rangeInInches<11)&&(rangeInInches>9)){
   digitalWrite (ledPin4, HIGH);
   delay (30000);
   digitalWrite (ledPin4, LOW);
  
}

if ((rangeInInches<13)&&(rangeInInches>11)){
   digitalWrite (ledPin5, HIGH);
   delay (30000);
   digitalWrite (ledPin5, LOW);
  
}

if ((rangeInInches<15)&&(rangeInInches>13)){
   digitalWrite (ledPin6, HIGH);
   delay (30000);
   digitalWrite (ledPin6, LOW);
  
}

    t = millis()/1000.0;

  for(i=0;i<PIXELCOUNT;i++) {
    
    scale[i] = 0.25*sin(2*M_PI*((FREQ*t) - (i/(float)PIXELCOUNT)))+0.3;
    pixel.setPixelColor(i,scale[i]*cred,scale[i]*cgreen,scale[i]*cblue);
  }
  pixel.show();
}