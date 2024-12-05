/* 
 * Project Livelights Capstone Project
 * Author: Caroline C. Blaker
 * Date: 2024-12-02 - 2024-12-13
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_VL53L0X.h"


int LOXL_ADDRESS = 0x30;
int LOXC_ADDRESS = 0x31;
int LOXR_ADDRESS = 0x32;
int LOXL_PIN = D5;
int LOXC_PIN = D6;
int LOXR_PIN = D7;
int max1 = 120,max2 = 230,max3 =370;

const int INTERVAL=2000;
int currentTime;
int lastTime;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

Adafruit_VL53L0X loxL = Adafruit_VL53L0X();
Adafruit_VL53L0X loxC = Adafruit_VL53L0X();
Adafruit_VL53L0X loxR = Adafruit_VL53L0X();
//SerialLogHandler logHandler(LOG_LEVEL_INFO);
VL53L0X_RangingMeasurementData_t measureL;
VL53L0X_RangingMeasurementData_t measureC;
VL53L0X_RangingMeasurementData_t measureR;
void setID();
int positioner();

void setID() {  //sets new address for each TOF sensor
  // all reset
  digitalWrite(LOXL_PIN, LOW);    
  digitalWrite(LOXC_PIN, LOW);
  digitalWrite(LOXR_PIN, LOW);
  delay(10);
  // all unreset
  digitalWrite(LOXL_PIN, HIGH);
  digitalWrite(LOXC_PIN, HIGH);
  digitalWrite(LOXR_PIN, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(LOXL_PIN, HIGH);
  digitalWrite(LOXC_PIN, LOW);
  digitalWrite(LOXR_PIN, LOW);

  delay(10);
  if (!loxL.begin(LOXL_ADDRESS)) {
    Serial.println(F("Failed to boot left VL53L0X"));
    while(1);
  }

  digitalWrite(LOXC_PIN, HIGH);
  // initing center
  if(!loxC.begin(LOXC_ADDRESS)) {
    Serial.println(F("Failed to boot center VL53L0X"));
    while(1);
  }
  delay(10);

  // activating Right
  digitalWrite(LOXR_PIN, HIGH);
  delay(10);

  //initing Right
  if(!loxR.begin(LOXR_ADDRESS)) {
    Serial.println(F("Failed to boot right VL53L0X"));
    while(1);
  }
  Serial.printf("All Connected\n");
}
void setup() {
  Wire.begin();
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

  pinMode(LOXL_PIN, OUTPUT);
  pinMode(LOXC_PIN, OUTPUT);
  pinMode(LOXR_PIN, OUTPUT);

  Serial.println("Shutdown pins inited...");


  digitalWrite(LOXL_PIN, LOW);
  digitalWrite(LOXC_PIN, LOW);
  digitalWrite(LOXR_PIN, LOW);

  Serial.println("Both in reset mode...(all 3 are low)");
  
  
  Serial.println("Starting...");
  setID();
  lastTime=millis();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  loxL.rangingTest(&measureL, false);
  if (millis()-lastTime > INTERVAL){
  int position=positioner();
  Serial.printf("%i\n",position);
  lastTime=millis();
  }
}
int positioner(){ // uses TOF to see which threshold is active 
  int handPos;  
 
  loxL.rangingTest(&measureL, true); // pass in 'true' to get debug data printout!
  loxC.rangingTest(&measureC, true);
  loxC.rangingTest(&measureR, true);
  handPos=0;
  return handPos;
}
