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
//#include "Grove-Ultrasonic-Ranger.h" removed and replaced with ToF due to neopixel and led flickers
#include <Adafruit_MQTT.h>
#include "credentials.h"
#include <Adafruit_MQTT/Adafruit_MQTT_SPARK.h>
#include <Adafruit_MQTT/Adafruit_MQTT.h>
#include "Adafruit_PWMServoDriver.h"
#include "Adafruit_VL53L0X.h"
#include "credentials.h"

//const int sigPin = A5;
const float FREQ = 0.2;
const int PIXELCOUNT = 120;
const int LIGHTCOLOR = 0xFF8B17; // Hex color for 2700K light
const int publishTime = 600000;
Adafruit_NeoPixel pixel (PIXELCOUNT, SPI, WS2812B);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

float t;
float scale[PIXELCOUNT];
int i;
int bri[9];
int briOn[9];
int cred, cgreen, cblue;
int personPosition();
int personLocation;
int segment;
int timeSincePerson; 
int lastPerson;

float personTimer[9];

int personDetected0;
int personDetected1;
int personDetected2;
int personDetected3;
int personDetected4;
int personDetected5;
int personDetected6;
int personDetected7;
int personDetected8;
int personDetected9;
int personDetected10;
int personDetected11;

int rangeInInches;

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

/************ Global State (you don't need to change this!) ***   ***************/ 
TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Subscribe buttonFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buttononoff"); 
Adafruit_MQTT_Publish counterFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/randomnumber");

/************Declare Variables*************/
unsigned int last, lastTime;
float subValue,pubValue;

/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();
void pixelWave();
float buttonOnOff;
float personCounter;

Thread thread("pixelwave", pixelWave);
SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);
  pwm.begin();
  pwm.setPWMFreq(1600);

  while (! Serial) { // wait until serial port opens for native USB devices
    delay(1);
  }
  delay(2000);
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to start VL53L0X"));
    while(1);
  }
 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));  // power 

 waitFor(Serial.isConnected,15000);

//Connect to Internet but not Particle Cloud
   WiFi.on();
   WiFi.connect();
   while(WiFi.connecting()) {
   Serial.printf(".");

  }

  Serial.printf("\n\n");

//Setup MQTT subscription
  mqtt.subscribe(&buttonFeed); 
  //mqtt.subscribe(&counterFeed); 

  pixel.begin();
  pixel.setBrightness(255);
  pixel.show();

  cred = LIGHTCOLOR >> 16;
  cgreen = LIGHTCOLOR >> 8 & 0xFF;
  cblue = LIGHTCOLOR & 0xFF;

  }


void loop() {
  MQTT_connect();
  MQTT_ping();

  Adafruit_MQTT_Subscribe *subscription;
   while ((subscription = mqtt.readSubscription(100))) {// changed from .readSubscription
     if (subscription == &buttonFeed) { //changed from &subFeed
       subValue = atof((char *)buttonFeed.lastread); //changed from subValue = atof((char*))subFeed.lastread);
     }
   }
   //turing thread on/off
  // if (subValue ==0) {
  //   SYSTEM_THREAD(DISABLED);
  //   Serial.printf("Lights Off\n");
  //   for (i=0;i<PIXELCOUNT;i++) {
  //   pixel.setPixelColor(i,0,0,0);
  //   pixel.show();
  // }
  // }
  // else{
  //   SYSTEM_THREAD(ENABLED);
  // }
    

  // Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.


  //personLocation = personPosition();
  //Serial.printf("Person Position:%i\r",personLocation);
       VL53L0X_RangingMeasurementData_t measure;
    //static int personPosition;  
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
    // if (measure.RangeStatus != 0) {  
      Serial.printf("mm: %i\n", measure.RangeMilliMeter);
  //measure.RangeMilliMeter;
      segment = measure.RangeMilliMeter/16;
    
  if (segment < 9){
  personTimer[segment]= millis();

  Serial.printf("Segment %i \n", segment);
  //Serial.printf("Person Timer %i\n", personTimer[i]);

for ( i=0; i<9; i++){
  timeSincePerson = ((millis()-personTimer[i])/1000.0);
  //Serial.printf("timeSincePerson %i\n", timeSincePerson);

  if (timeSincePerson <30){
    briOn[i]= 4096*(1-(timeSincePerson/30.0));
    bri[i] = 0;
    Serial.printf("bri %i\n", bri[i]);
  }
  else{
    briOn[i]= 0;
    bri[i] = 4096;
  }
  }
  }
  for ( i=0; i<12; i++){
    //for(int ii = 0; ii < 9; ii++){
  pwm.setPWM(i,briOn[i],bri[i]);
  }
  if(measure.RangeMilliMeter <200) {
      personCounter=1;  
  } 
  else {
  personCounter=0;
  }
  if ((millis() - lastPerson) > 3600000) {
    if (mqtt.Update()){
        counterFeed.publish(personCounter);
        lastPerson = millis();
      }
  }
  Serial.printf("Publishing %0.2f \n",personCounter); 
}

//}

//}

void pixelWave(){
  float t;
  int i;
  float scale[PIXELCOUNT];

while(true){

  t = millis()/1000.0;
  for(i=0;i<PIXELCOUNT;i++) {
    scale[i] = 0.25*sin(2*M_PI*((FREQ*t) - (i/(float)PIXELCOUNT)))+0.3;
    pixel.setPixelColor(i,scale[i]*cred,scale[i]*cgreen,scale[i]*cblue);
  }
  pixel.show();
}
}

void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) { //checks to see if connected to function
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}




  

//   int personPosition(){
//     VL53L0X_RangingMeasurementData_t measure;
//     static int personPosition;  
//     lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
//     if (measure.RangeStatus != 0) {  
//     if(measure.RangeMilliMeter <= 2){
//      if (personPosition){
//       personTimer0 = millis();
//     }

//     timeSincePerson0 = (millis()-personTimer0)/1000.0;
//     if (timeSincePerson0<30){
//       bri = 4095*(1-(timeSincePerson0/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(0,0,bri);
//     }

//     else if(measure.RangeMilliMeter >2 && measure.RangeMilliMeter <=4){
//       if (personPosition){
//         personTimer1 = millis();
//       }

//     timeSincePerson1 = (millis()-personTimer1)/1000.0;
//     if (timeSincePerson1<30){
//       bri = 4095*(1-(timeSincePerson1/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(1,0,bri);
//     }

//     else if(measure.RangeMilliMeter >4 && measure.RangeMilliMeter <=6){
//       if (personDetected2){
//         personTimer2 = millis();
//       }

//     timeSincePerson2 = (millis()-personTimer2)/1000.0;
//     if (timeSincePerson2<30){
//       bri = 4095*(1-(timeSincePerson2/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(2,0,bri);
//     }

//     else if(measure.RangeMilliMeter >6 && measure.RangeMilliMeter <=8){
//       if (personDetected3){
//         personTimer3 = millis();
//       }

//     timeSincePerson3 = (millis()-personTimer3)/1000.0;
//     if (timeSincePerson3<30){
//       bri = 4095*(1-(timeSincePerson3/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(3,0,bri);
//       }

//     else if(measure.RangeMilliMeter >8 && measure.RangeMilliMeter <=10){
//       if (personDetected4){
//         personTimer4 = millis();
//      }

//     timeSincePerson4 = (millis()-personTimer4)/1000.0;
//     if (timeSincePerson4<30){
//       bri = 4095*(1-(timeSincePerson4/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(4,0,bri);
//       }

//     else if(measure.RangeMilliMeter >10 && measure.RangeMilliMeter <=12){
//       if (personDetected5){
//         personTimer5 = millis();
//       }

//     timeSincePerson5 = (millis()-personTimer5)/1000.0;
//     if (timeSincePerson5<30){
//       bri = 4095*(1-(timeSincePerson5/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(5,0,bri);
//       }

//     else if(measure.RangeMilliMeter >14 && measure.RangeMilliMeter <=16){
//       if (personDetected6){
//         personTimer6 = millis();
//     }

//     timeSincePerson6 = (millis()-personTimer6)/1000.0;
//     if (timeSincePerson6<30){
//       bri = 4095*(1-(timeSincePerson6/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(6,0,bri);
//       }

//     else if(measure.RangeMilliMeter >18 && measure.RangeMilliMeter <=20){
//       if (personDetected7){
//         personTimer7 = millis();
//       }

//     timeSincePerson7 = (millis()-personTimer7)/1000.0;
//     if (timeSincePerson7<30){
//       bri = 4095*(1-(timeSincePerson7/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(7,0,bri);
//       }

//     else if(measure.RangeMilliMeter >20 && measure.RangeMilliMeter <=22){
//       if (personDetected8){
//         personTimer8 = millis();
//       }

//     timeSincePerson8 = (millis()-personTimer8)/1000.0;
//     if (timeSincePerson8<30){
//       bri = 4095*(1-(timeSincePerson8/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(8,0,bri);
//       }

//     else if(measure.RangeMilliMeter >22 && measure.RangeMilliMeter <=24){
//       if (personDetected9){
//         personTimer9 = millis();
//       }

//     timeSincePerson9 = (millis()-personTimer9)/1000.0;
//     if (timeSincePerson9<30){
//       bri = 4095*(1-(timeSincePerson9/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(9,0,bri);
//       }

//     else if(measure.RangeMilliMeter >26 && measure.RangeMilliMeter <=28){
//       if (personDetected10){
//         personTimer10 = millis();
//      }

//     timeSincePerson10 = (millis()-personTimer10)/1000.0;
//     if (timeSincePerson10<30){
//       bri = 4095*(1-(timeSincePerson10/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(10,0,bri);
//       }

//     else if(measure.RangeMilliMeter >28 && measure.RangeMilliMeter <=30){
//       if (personDetected11){
//         personTimer11 = millis();
//       }

//     timeSincePerson11 = (millis()-personTimer11)/1000.0;
//     if (timeSincePerson11<30){
//       bri = 4095*(1-(timeSincePerson11/30.0));
//     }

//     else {
//       bri = 0;
//     }

//    pwm.setPWM(11,0,bri);
//   } 
  
// }
// return measure.RangeMilliMeter;
//   }