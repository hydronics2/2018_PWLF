//
// NOTES:
// Change Box Cluster Number to 0-15... each station is made up of 2 box clusters
// 0,1
// 2,3
// 4,5 and so on to 10,11
// even number box clusters are on the left, odd number on the right
// individual boxes within the box clusters start top left as 0 through bottom left as 7 zig zag
//
// Box Cluster Number -  2 numbers  5,6,7 are super sensative.

#define boxClusterNumber 5

#include "Keyboard.h"
#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include "Color.h"

#define PIN 2
#define BAUD_RATE     (115200)

// DEBUG_SERIAL must be false for production!
#define DEBUG_SERIAL  (false)

#define randf()    (random(0xffffff) * (1.0f / 0xffffff))

#define ATTRACTOR_FADE_OUT_TIME       (1.6f)
#define ATTRACTOR_HIDE_TIME           (4.5f)
#define ATTRACTOR_FADE_IN_TIME        (3.5f)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_RBG + NEO_KHZ800);

int box[][8] = {      //this fixes all the piezo locations for each box. They were randomly connected and have to be asigned to the correct box
  {3,0,7,9,6,8,2,1}, //box 0
  {1,0,2,8,9,7,3,6},  //box 1
  {9,7,1,0,2,8,6,3},  //box 2
  {9,1,0,2,8,7,3,6},  //box 3
  {0,9,2,1,8,3,7,6},  //box 4
  {6,7,8,2,3,9,1,0}  //box 5
}; //box 1

#define HT    (15)      // Default hit threshold

int hitThresholdAfterDelay[][12] = {      //calibration if needed.. default was 20
  {HT,HT,HT,HT,HT,HT,HT,HT}, //box 0
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 1
  {HT,HT,HT,HT,HT,70,70,50},  //box 2  - boxes 5,6,7 were extra sensative...
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 3
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 4
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 5
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 6
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 7
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 8
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 9
  {HT,HT,HT,HT,HT,HT,HT,HT},  //box 10
  {HT,HT,HT,HT,HT,HT,HT,HT}  //box 11
}; //box 1

#define MM     (40)     // Default millis before measurement

int millisBeforeMeasurement[12] = {  //callibration if needed ... default was 58 boxes 2 and 3 were sensitive
  MM,MM,MM,MM,MM,MM,
  MM,MM,MM,MM,MM,MM
};

uint16_t ledArray[]={0,2,4,6,8,10,12,14};

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

HSV hsvRandoms[8];
float brights[8] = {0.0f};

void setup() {

  pinMode(0, OUTPUT);
  digitalWrite(0,LOW);

  pinMode(5,OUTPUT);   //receive / transmit pin... pull high to send
  digitalWrite(5, LOW);

  if (DEBUG_SERIAL) {
    Serial.begin(BAUD_RATE);
  }

  Serial1.begin(BAUD_RATE);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

int captureData = 254;
const int hitThreshold = 30;

long piezoLastHit[8];
long currentTime = 0;
const int waitBetweenHits = 100;

float elapsed = 0.0f;
float attractor = 1.0f; // Attractor animation fades in/out as appropriate. 0==off, 1==on.
float attractorFadeOut = 0.0f;  // When a piezo is hit: Set this to fade out attractor.

void loop() {
  long lastTime = currentTime;
  currentTime = millis();

  float frameSeconds = (currentTime - lastTime) / 1000.0f;
  elapsed += frameSeconds;

  if (attractorFadeOut > 0.0f) {
    // Attractor is fading out / hiding
    attractor = max(0.0f, attractor - (frameSeconds / ATTRACTOR_FADE_OUT_TIME));
    attractorFadeOut -= frameSeconds;

  } else {
    // Attractor is fading in
    attractor = min(1.0f, attractor + (frameSeconds / ATTRACTOR_FADE_IN_TIME));
  }

  // For each box: Dim the colors according to the brights.
  // Update each LED color.
  boxColorsWillUpdate(boxClusterNumber, elapsed, attractor);

  for (uint8_t box = 0; box < 8; box++) {
    // Get the box color. See: Color.h
    RGB rgb = boxColor(boxClusterNumber, box, &hsvRandoms[box], brights[box], elapsed, attractor);

    RGB8 rgb8 = rgbToRGB8(rgb);

    uint16_t stripNum = ledArray[box];
    strip.setPixelColor(stripNum, rgb8.r, rgb8.g, rgb8.b);
    strip.setPixelColor(stripNum + 1, rgb8.r, rgb8.g, rgb8.b);

    // Decay brightness
    brights[box] = max(0.0f, brights[box] - (1.0f / 2000.0f));
  }

  strip.show();

  // No piezo has been triggered:
  if(captureData > 8)
  {
    for (uint8_t b = 0; b < 8; b++) {
      // If piezo was tapped: isPiezoTriggered() sets captureData value.
      if (isPiezoTriggered(b)) return;
    }
  }

  //once triggered... read only active sensor

  if(captureData < 8) //
  {
    // After an interval of millis, we can read the sensor, to determine
    // whether there's enough energy to constitute a hit.
    if ((currentTime - piezoLastHit[captureData]) >= millisBeforeMeasurement[boxClusterNumber]) {

      sensorValue = analogRead(box[boxClusterNumber][captureData]);

      if (DEBUG_SERIAL) {
        Serial.print("decayed:");
        Serial.println(sensorValue);
      }

      if (sensorValue >= hitThresholdAfterDelay[boxClusterNumber][captureData]) {
        // Hit confirmed!
        processHit(sensorValue);
      }

      // Hit has been processed, now it's safe to reset captureData
      captureData = 254;
    }
  }
}

// Returns true if a piezo has been triggered.
bool isPiezoTriggered(uint8_t b)
{
  sensorValue = analogRead(box[boxClusterNumber][b]);
  if(sensorValue > hitThreshold)
  {
    if (DEBUG_SERIAL) {
      Serial.print("triggered:");
      Serial.println(sensorValue);
    }

    if(currentTime - piezoLastHit[b] > waitBetweenHits)
    {
      piezoLastHit[b] = currentTime;
      captureData = b;
      return true;
    }
  }

  return false;
}

void processHit(int sensorValue)
{
  // Take control of the data line (RS485 differential pair)
  digitalWrite(5, HIGH);

  Serial1.print(boxClusterNumber + 10);
  Serial1.print(",");
  Serial1.println(captureData);

  if (DEBUG_SERIAL) {
    Serial.print(boxClusterNumber + 10);
    Serial.print(",");
    Serial.println(captureData);
    Serial.println(sensorValue);
  }

  // Relinquish control of the data line
  Serial1.flush();

  if (DEBUG_SERIAL) {
    Serial.flush();
  }

  digitalWrite(5, LOW);

  startBoxAnimation(captureData);
}

// Fill the dots one after the other with a color
void startBoxAnimation(uint8_t b) {
  // Fade out the attractor. Pad with time for the attractor to stay hidden.
  attractorFadeOut = ATTRACTOR_FADE_OUT_TIME + ATTRACTOR_HIDE_TIME;

  //colors[b] = Wheel(random(0, 255));
  hsvRandoms[b] = (HSV){.h = randf(), .s = randf(), .v = randf()};
  brights[b] = 1.0f;  // Start at full brightness

  boxColorWasHit(boxClusterNumber, b);
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
