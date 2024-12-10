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

// this naming convention diverges from the styleguide in a way
// that helps me keep track of what the variable means.
const int LOXL_MAX = 430; 
const int bLOXL_MAX= 390;
const int LOXC_MAX = 310; 
const int bLOXC_MAX= 400;
const int LOXR_MAX = 330; 
const int bLOXR_MAX= 400;

int i, j,n;

const int INTERVAL=500;
const int ONCEASOMETHING= 2000;
int currentTime;
int lastTime;

uint8_t wait;

const int red = 0;
const int orange = (30.0/360.0)*255;
const int yellow = (60.0/360.0)*255;
const int green = (120.0/360.0)*255;
const int blue = (240.0/360.0)*255;
const int indigo = (265.0/360.0)*255;
const int violet = (275.0/360.0)*255;
const int TOTAL_NEOPIXELS = 300;
const int rainbow2[] = {red, orange, yellow, green, blue,indigo,violet};

int pointX,colorband, pointY,brightVal,randomColor, start;


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

int c_val[]= {4, 5, 6, 7, 8, 9, 10};
int r_val[]= {8, 9, 10, 11, 12, 13, 14};
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
void createDots();
void pixelFill(int start, int end, int color);
void matrixFill(int color);
void lineRunner(int posx, int posy,int color);
void rainbow(uint8_t wait);
uint32_t hueWheel(byte WheelPos);
byte reverseWheel(uint32_t color);
void setPixel( uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness);

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
   n=0;
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // Does Art Loop

  //Gets Interrupted, does user show
  loxR.rangingTest(&measureR, false);
  bLoxR.rangingTest(&bMeasureR, false);
  loxC.rangingTest(&measureC, false);
  bLoxC.rangingTest(&bMeasureC, false);
  loxL.rangingTest(&measureL, false);
  bLoxL.rangingTest(&bMeasureL, false);

  if (millis()-lastTime > ONCEASOMETHING){
    start = positioner();
    Serial.printf("start: %i\n",start);
    createDots();
    if(bMeasureR.RangeMilliMeter<bLOXR_MAX || measureR.RangeMilliMeter<LOXR_MAX){
    Serial.printf(" Right Top: %i, bottom :%i\n",measureR.RangeMilliMeter,bMeasureR.RangeMilliMeter);
    }
    if(bMeasureC.RangeMilliMeter<bLOXC_MAX || measureC.RangeMilliMeter<LOXC_MAX){
    Serial.printf(" Center Top: %i, bottom :%i\n",measureC.RangeMilliMeter,bMeasureC.RangeMilliMeter);
    }
    if(bMeasureL.RangeMilliMeter<bLOXL_MAX || measureL.RangeMilliMeter<LOXL_MAX){
    Serial.printf(" Left Top: %i, bottom :%i\n",measureL.RangeMilliMeter,bMeasureL.RangeMilliMeter);
    }
    // Try other single-pixel-origin things here
    //lineRunner(pointX,pointY,rainbow2[random(9)]);
    //panel.setPixelColor(myMatrix[pointX][pointY],rainbow2[random(9)]);
    lastTime=millis();
  }
}
void createDots(){
  byte r,g,b;
  pointX= random(20);
  pointY= random(15);
  brightVal=random(75,150);
  //randomColor= hueWheel(rainbow2[random(6)]);
    
  // Bitwise op for rgb
  //r = randomColor>>16 & 0xFF;
  //g = randomColor>>8 & 0xFF;
  //b = randomColor & 0xFF;

  r= random(255);
  g= random(255);
  b= random(255);
  Serial.printf("Color: R: %i, G: %i, B: %i\n",r,g,b);
  setPixel(myMatrix[pointX][pointY],r,g,b,brightVal);
    
  }
void setPixel( uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness) {
	panel.setPixelColor(n, (brightness*r/255) , (brightness*g/255), (brightness*b/255));
  panel.show();
}
  

void lineRunner(int pos_x,int pos_y,int color){
 // myRow=myMatrix[pointX];

}
// Super medium-fidelity person-position tracker
int positioner(){ // uses TOF to see which threshold is active 
  int handPos;
  int x_val; 
  int y_val;
  //If we have any reads within the boundaries of the max
  if ((bMeasureR.RangeMilliMeter<bLOXR_MAX)||bMeasureC.RangeMilliMeter<bLOXC_MAX||bMeasureL.RangeMilliMeter<bLOXL_MAX) {
    // Let's compare left and right
    if(bMeasureL.RangeMilliMeter > bMeasureR.RangeMilliMeter) {
      // and then center to left
      if(bMeasureL.RangeMilliMeter > bMeasureC.RangeMilliMeter){
        x_val= 0;
        if (measureL.RangeMilliMeter <300){
          y_val = 6;
        } else if (measureL.RangeMilliMeter <400){
          y_val = 9;
        }else if (measureL.RangeMilliMeter <500){
          y_val = 11;
        } else {
          y_val= 16;
        }
      } else {
        x_val= 4;
        if (measureC.RangeMilliMeter <300){
          y_val = 6;
        } else if (measureC.RangeMilliMeter <400){
          y_val = 9;
        }else if (measureC.RangeMilliMeter <500){
          y_val = 11;
        } else {
          y_val= 16;
        }
        }
      } else {
        if(bMeasureR.RangeMilliMeter > bMeasureC.RangeMilliMeter){
        x_val= 8;
        if (measureR.RangeMilliMeter <300){
          y_val = 6;
        } else if (measureR.RangeMilliMeter <400){
          y_val = 9;
        }else if (measureR.RangeMilliMeter <500){
          y_val = 11;
        } else {
          y_val= 16;
        }
        } else {
        x_val= 4;
        if (measureC.RangeMilliMeter <300){
          y_val = 6;
        } else if (measureC.RangeMilliMeter <400){
          y_val = 9;
        }
        else if (measureC.RangeMilliMeter <500){
          y_val = 11;
        } else {
          y_val= 16;
        }
        }
      }
    handPos=  myMatrix[random(x_val,6)][y_val];
    
    }else {
    handPos=-1;
    }
    // the interesting value is on the left, center, or right

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
      delay(10);
    }
  }
}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<panel.numPixels(); i++) {
      panel.setPixelColor(i, hueWheel((i+j) & 255));
    }
    panel.show();
    delay(wait);
  }
}
// Convert hue to rgb (hue values 0 - 255 equal 0 to 360 degrees)
uint32_t hueWheel(byte WheelPos) {
  if(WheelPos < 85) {
   return panel.Color(255 - WheelPos * 3, WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return panel.Color( 0, 255 - WheelPos * 3, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return panel.Color( WheelPos * 3, 0, 255 - WheelPos * 3);
  }
}

// Convert rgb hexcolor to hue
byte reverseWheel(uint32_t color) {
  byte r,g,b;

  r = color>>16 & 0xFF;
  g = color>>8 & 0xFF;
  b = color & 0xFF;

  if(b==0) {
    return (g/3);
  }

  if(g==0) {
    return ((r/3)+170);
  }

  if(r==0) {
    return ((b/3)+85);
  }

  return -1;
}