//
// NOTES:
// Box 2 numbers  5,6,7 are super sensative. 


#define boxNumber 2

#include "Keyboard.h"

#include <Adafruit_NeoPixel.h>
#define PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

int box[][8] = {      //this fixes all the piezo locations for each box. They were randomly connected and have to be asigned to the correct box
  {3,0,7,9,6,8,2,1}, //box 0 
  {1,0,2,8,9,7,3,6},  //box 1
  {9,7,1,0,2,8,6,3},  //box 2
  {9,1,0,2,8,7,3,6}  //box 3
}; //box 1


int hitThresholdAfterDelay[][8] = {      //calibration if needed.. default was 20 
  {20,20,20,20,20,20,20,20}, //box 0 
  {20,20,20,20,20,20,20,20},  //box 1
  {20,20,20,20,20,70,70,50},  //box 2  - boxes 5,6,7 were extra sensative...
  {20,20,20,20,20,20,20,20}  //box 3
}; //box 1

int arrayDelayIncrement[8] = {  //callibration if needed ... default was 58 boxes 2 and 3 were sensitive
  58,58,70,65,58,58,58,58
};



int ledArray[]={0,2,4,6,8,10,12,14};

char keyboard[] = {'m','n','b','v','a','s','d','f'};

byte pixelColors[][3]={
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0}
};

int sensorValue = 0;        // value read from the pot

void setup() {

  pinMode(0, OUTPUT);
  digitalWrite(0,LOW);

  pinMode(5,OUTPUT);   //receive / transmit pin... pull high to send
  digitalWrite(5, HIGH);
  

 Serial.begin(9600);
 Serial1.begin(9600);


  //Keyboard.begin();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}


const int sizeRolling = 100; ///this needs to be high to accomidate all the arrayDelayIncrement[boxNumber].... low is 58... high is?
int rollingWave[sizeRolling];
int incrementRolling = 2;

int captureData = 254;
int hitThreshold = 30;

long piezoLastHit[8];
long currentTime = 0;
int waitBetweenHits = 100;


void loop() {

  if(captureData > 8)
  {
    currentTime = millis();
    sensorValue = analogRead(box[boxNumber][0]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[0] > waitBetweenHits)
      {
        piezoLastHit[0] = currentTime;
        captureData = 0;
        addToArray(sensorValue);
        return;
      }
    }
    sensorValue = analogRead(box[boxNumber][1]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[1] > waitBetweenHits)
      {
        piezoLastHit[1] = currentTime;
        captureData = 1;
        addToArray(sensorValue);
        return;
      }
    }

    sensorValue = analogRead(box[boxNumber][2]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[2] > waitBetweenHits)
      {
        piezoLastHit[2] = currentTime;
        captureData = 2;
        addToArray(sensorValue);
        return;
      }
    }

    sensorValue = analogRead(box[boxNumber][3]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[3] > waitBetweenHits)
      {
        piezoLastHit[3] = currentTime;
        captureData = 3;
        addToArray(sensorValue);
        return;
      }
    }

    sensorValue = analogRead(box[boxNumber][4]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[4] > waitBetweenHits)
      {
        piezoLastHit[4] = currentTime;
        captureData = 4;
        addToArray(sensorValue);
      return;
      }
    }

    sensorValue = analogRead(box[boxNumber][5]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[5] > waitBetweenHits)
      {
        piezoLastHit[5] = currentTime;
        captureData = 5;
        addToArray(sensorValue);
        return;
      }
    }

    sensorValue = analogRead(box[boxNumber][6]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[6] > waitBetweenHits)
      {
        piezoLastHit[6] = currentTime;
        captureData = 6;
        addToArray(sensorValue);
        return;
      }
    }

    sensorValue = analogRead(box[boxNumber][7]); //
    if(sensorValue > hitThreshold)//
    {
      if(currentTime - piezoLastHit[7] > waitBetweenHits)
      {
        piezoLastHit[7] = currentTime;
        captureData = 7;
        addToArray(sensorValue);
        return;
      }
    }
  }

  //once triggered... read only active sensor

  if(captureData < 8) //
  {
    delayMicroseconds(200);
    switch (captureData) 
    {
      case 0:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
      case 1:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
      case 2:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
      case 3:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
      case 4:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
      case 5:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
      case 6:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
      case 7:
        sensorValue = analogRead(box[boxNumber][captureData]); //
        addToArray(sensorValue);
        break;
    }
  }
  
}

void addToArray(int sensorValue)
{
  rollingWave[incrementRolling] = sensorValue;      //keeps track of thrown or dropped
  incrementRolling++;
  if(incrementRolling == arrayDelayIncrement[boxNumber]+2)
  {
    
    incrementRolling = 0;

//    for (int i=0; i<sizeRolling; i++)  //find the first zero
//    {
//      Serial.print(i);
//      Serial.print(", ");
//      Serial.println(rollingWave[i]);
//    }

    if(rollingWave[arrayDelayIncrement[boxNumber]] > hitThresholdAfterDelay[boxNumber][captureData])
    {
      
      Serial1.print(boxNumber + 10);
      Serial1.print(",");
      Serial1.println(captureData);

      Serial.print(boxNumber + 10);
      Serial.print(",");
      Serial.println(captureData);
      //Serial.println(rollingWave[arrayDelayIncrement[boxNumber]]);
      

      colorWipe(Wheel(random(0,254),captureData),ledArray[captureData]);
      captureData = 254;

    }
    captureData = 254;
  }
}


void findAverageRolling(){
  int highValue = 0;
  int numberOf = 0;
  int byteToSend = 0;
  for(int i = 0; i < sizeRolling; i++){
//    if(rollingAcc[i] > 500){
//      numberOf++;
//      highValue = highValue + rollingAcc[i];
//    }
  }
  if(numberOf > 0){
    byteToSend = highValue/numberOf;
    //return (byte)byteToSend;
  }else{
    byteToSend = 0;
    //return (byte)byteToSend;    
  }
}




// Fill the dots one after the other with a color
void colorWipe(uint32_t c,byte stripNum) {
  //Serial.println(stripNum);
    strip.setPixelColor(stripNum, c);
    strip.setPixelColor(stripNum+1, c);
    strip.show(); 
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos,byte pixel) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    pixelColors[pixel][0] = 255 - WheelPos * 3;
    pixelColors[pixel][1] = 0;
    pixelColors[pixel][2] = WheelPos * 3;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    pixelColors[pixel][0] = 0;
    pixelColors[pixel][1] = WheelPos * 3;
    pixelColors[pixel][2] = 255 - WheelPos * 3;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  pixelColors[pixel][0] = WheelPos * 3;
  pixelColors[pixel][1] = 255 - WheelPos * 3;
  pixelColors[pixel][2] = 0;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
