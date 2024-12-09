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
#include "neopixel.h"
#include "colors.h"


int LOXL_ADDRESS = 0x30;
int LOXC_ADDRESS = 0x31;
int LOXR_ADDRESS = 0x32;
int bLOXL_ADDRESS = 0x33;
int bLOXC_ADDRESS = 0x34;
int bLOXR_ADDRESS = 0x35;
int LOXL_PIN = D5;
int LOXC_PIN = D6;
int LOXR_PIN = D7;
int bLOXL_PIN = D12;
int bLOXC_PIN = D13;
int bLOXR_PIN = D14;
const int LOXL_MAX = 360; // change when calibrated
const int LOXC_MAX = 475; // change when calibrated
const int LOXR_MAX = 300; //change when calibrated
int i, j;

const int INTERVAL=500;
int currentTime;
int lastTime;

const int TOTAL_NEOPIXELS = 300;
const int rainbow2[] = {red, 0xAA4500, yellow, green, blue,0x3900c9,violet,salmon, tomato,chocolate, };
int colorband;
int pointX, pointY;
//                                                             10th                                             20th
int myMatrix[20][15] = {{19, 20,  59,  60,  99,  100,  139,  140,  179,  180,  219,  220,  259,  260, 299},  
                   { 18, 21,  58,  61,  98,  101,  138,  141,  178,  181,  218,  221,  258,  261, 298},  
                   { 17, 22,  57,  62,  97,  102,  137,  142,  177,  182,  217,  222,  257,  262, 297},  
                   { 16, 23,  56,  63,  96,  103,  136,  143,  176,  183,  216,  223,  256,  263, 296},
                   { 15, 24,  55,  64,  95,  104,  135,  144,  175,  184,  215,  224,  255,  264, 295},
                   { 14, 25,  54,  65,  94,  105,  134,  145,  174,  185,  214,  225,  254,  265, 294},
                   { 13, 26,  53,  66,  93,  106,  133,  146,  173,  186,  213,  226,  253,  266, 293},
                   { 12, 27,  52,  67,  92,  107,  132,  147,  172,  187,  212,  227,  252,  267, 292},
                   { 11, 28,  51,  68,  91,  108,  131,  148,  171,  188,  211,  228,  251,  268, 291},
                   { 10, 29,  50,  69,  90,  109,  130,  149,  170,  189,  210,  229,  250,  269, 290},
                   {  9, 30,  49,  70,  89,  110,  129,  150,  169,  190,  209,  230,  249,  270, 289},
                   {  8, 31,  48,  71,  88,  111,  128,  151,  168,  191,  208,  231,  248,  271, 288},
                   {  7, 32,  47,  72,  87,  112,  127,  152,  167,  192,  207,  232,  247,  272, 287},
                   {  6, 33,  46,  73,  86,  113,  126,  153,  166,  193,  206,  233,  246,  273, 286},
                   {  5, 34,  45,  74,  85,  114,  125,  154,  165,  194,  205,  234,  245,  274, 285},
                   {  4, 35,  44,  75,  84,  115,  124,  155,  164,  195,  204,  235,  244,  275, 284},
                   {  3, 36,  43,  76,  83,  116,  123,  156,  163,  196,  203,  236,  243,  276, 283},
                   {  2, 37,  42,  77,  82,  117,  122,  157,  162,  197,  202,  237,  242,  277, 282},
                   {  1, 38,  41,  78,  81,  118,  121,  158,  161,  198,  201,  238,  241,  278, 281},
                   {  0, 39,  40,  79,  80,  119,  120,  159,  160,  199,  200,  239,  240,  279, 280}};

int myColors[20][15];
// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

Adafruit_NeoPixel panel(TOTAL_NEOPIXELS, SPI1, WS2812B);


Adafruit_VL53L0X loxL = Adafruit_VL53L0X();
Adafruit_VL53L0X loxC = Adafruit_VL53L0X();
Adafruit_VL53L0X loxR = Adafruit_VL53L0X();
Adafruit_VL53L0X bLoxL = Adafruit_VL53L0X();
Adafruit_VL53L0X bLoxC = Adafruit_VL53L0X();
Adafruit_VL53L0X bLoxR = Adafruit_VL53L0X();
//SerialLogHandler logHandler(LOG_LEVEL_INFO);
VL53L0X_RangingMeasurementData_t measureL;
VL53L0X_RangingMeasurementData_t measureC;
VL53L0X_RangingMeasurementData_t measureR;
VL53L0X_RangingMeasurementData_t bMeasureL;
VL53L0X_RangingMeasurementData_t bMeasureC;
VL53L0X_RangingMeasurementData_t bMeasureR;

void setID();
int positioner();
void pixelFill(int start, int end, int color);
void matrixFill(int color);
void lineRunner(int posx, int posy,int color);

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
  Serial.printf("going LOW\n");
  // all reset
  digitalWrite(bLOXL_PIN, LOW);    
  digitalWrite(bLOXC_PIN, LOW);
  digitalWrite(bLOXR_PIN, LOW);
  delay(10);
  // all unreset
  digitalWrite(bLOXL_PIN, HIGH);
  digitalWrite(bLOXC_PIN, HIGH);
  digitalWrite(bLOXR_PIN, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(bLOXL_PIN, HIGH);
  digitalWrite(bLOXC_PIN, LOW);
  digitalWrite(bLOXR_PIN, LOW);
    delay(10);
  if (!bLoxL.begin(bLOXL_ADDRESS)) {
    Serial.println(F("Failed to boot bottom left VL53L0X"));
    while(1);
  }
  delay(10);
   digitalWrite(bLOXC_PIN, HIGH);
  // initing center
  if(!bLoxC.begin(bLOXC_ADDRESS)) {
    Serial.println(F("Failed to boot center VL53L0X"));
    while(1);
  }
  delay(10);

  // activating Right
  digitalWrite(bLOXR_PIN, HIGH);
  delay(10);

  //initing Right
  if(!bLoxR.begin(bLOXR_ADDRESS)) {
    Serial.println(F("Failed to boot right VL53L0X"));
    while(1);
  }
  Serial.printf("All Connected\n");
}
void setup() {
  Wire.begin();
  Serial.begin(9600);
  panel.begin();
  panel.setBrightness(37);
  panel.show(); //initialize all off
  waitFor(Serial.isConnected,10000);

  pinMode(LOXL_PIN, OUTPUT);
  pinMode(LOXC_PIN, OUTPUT);
  pinMode(LOXR_PIN, OUTPUT);
  pinMode(bLOXL_PIN, OUTPUT);
  pinMode(bLOXC_PIN, OUTPUT);
  pinMode(bLOXR_PIN, OUTPUT);

  Serial.println("Shutdown pins inited...");


  digitalWrite(LOXL_PIN, LOW);
  digitalWrite(LOXC_PIN, LOW);
  digitalWrite(LOXR_PIN, LOW);
  digitalWrite(bLOXL_PIN, LOW);
  digitalWrite(bLOXC_PIN, LOW);
  digitalWrite(bLOXR_PIN, LOW);

  Serial.println("Both in reset mode...(all 3 are low)");
  
  
  Serial.println("Starting...");
  setID();
  lastTime=millis();
   matrixFill(rainbow2[5]);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // Does Art Loop
for (colorband=0;colorband<100;colorband++){
     // int _start = 3* colorband %10;
     
     // Serial.printf('Start: %i, end %i, ')
  }
  //Gets Interrupted, does user show
 // loxL.rangingTest(&measureL, false);
  if (millis()-lastTime > INTERVAL){
    pointX= random(20);
    pointY= random(15);
    panel.setBrightness(random(33,40));
    lastTime=millis();
    // Try other single-pixel-origin things here
    lineRunner(pointX,pointY,rainbow2[random(9)]);
    //panel.setPixelColor(myMatrix[pointX][pointY],rainbow2[random(9)]);
    panel.show();
  }
}
void lineRunner(int pos_x,int pos_y,int color){
 // myRow=myMatrix[pointX];

}
int positioner(){ // uses TOF to see which threshold is active 
  int handPos;  
 
  loxL.rangingTest(&measureL, true); // pass in 'true' to get debug data printout!
  loxC.rangingTest(&measureC, true);
  loxR.rangingTest(&measureR, true);
  handPos=0;
  return handPos;
}
void  pixelFill(int start, int end, int color) {
    int _i;
    for (_i = start; _i <= end; _i++ ){
        panel.setPixelColor(_i, color);
    }
    panel.show();
    
  }
void matrixFill(int color){
  for(i=0;i<20;i++) {
    for(j=0;j<15;j++) {
      panel.setPixelColor(myMatrix[i][j],color);
      panel.show();
      delay(50);
    }
  }
}
