//mnnbbbnnmmnnnbnbnnbnnnbnnnnnnnnnnnnnnnnnnnnnnnnnnnnnbbbn

#include "Keyboard.h"

#include <Adafruit_NeoPixel.h>
#define PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

const int piezo0 = A0;  
const int piezo7 = A1; 
const int piezo6 = A2;  
const int piezo5 = A3; 
const int piezo2 = A8; 
const int piezo3 = A9; 
const int piezo4 = A6; 
const int piezo1 = A7; 

int ledArray[]={8,10,12,14,0,2,4,6};

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

// Serial.begin(115200);
//
//    while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
  Keyboard.begin();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {

  sensorValue = analogRead(piezo0); // 1
  if(sensorValue > 60)//2-75
  {
    printData(sensorValue,0);
  }

  sensorValue = analogRead(piezo1);//  #8
  if(sensorValue > 100)
  {
    printData(sensorValue,1);
  }

  sensorValue = analogRead(piezo2);//
  if(sensorValue > 30) 
  {
    printData(sensorValue,2);
  }

  sensorValue = analogRead(piezo3); //#6 high threshold
  if(sensorValue > 120)
  {
    printData(sensorValue,3);
  }

  sensorValue = analogRead(piezo4);
  if(sensorValue > 60)
  {
    printData(sensorValue,4);
  }

  sensorValue = analogRead(piezo5); 
  if(sensorValue > 75) //was 120
  {
    printData(sensorValue,5);
  }

  sensorValue = analogRead(piezo6); 
  if(sensorValue > 400) 
  {
    printData(sensorValue,6);
  }

  sensorValue = analogRead(piezo7);  
  if(sensorValue > 70)
  {
    printData(sensorValue,7);
  }
    
//  for(int i = 0; i<8; i++)
//  {
//  if(pixelColors[i][0] > 0 || pixelColors[i][1] > 0 || pixelColors[i][2] > 0)
//    {
//      for(int j = 0; j<3; j++)
//      {
//        if(pixelColors[i][j] > 0)
//        {
//          pixelColors[i][j] = pixelColors[i][j] - 1;
//        }
//      }
//      if(i != 4){
//        colorWipe(strip.Color(pixelColors[i][0], pixelColors[i][1], pixelColors[i][2]),ledArray[i]);
//      }
//    }
//  }
}

void printData(int value, int pixel)
{
  Serial.print("sensor");
  Serial.print(pixel);
  Serial.print(": ");
  Serial.println(value);
  colorWipe(Wheel(random(0,255),pixel),ledArray[pixel]);
  Keyboard.print(keyboard[pixel]);
  delay(14);

  
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
