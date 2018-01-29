//
// NOTES:
// Change Box Cluster Number to 0-15... each station is made up of 2 box clusters
// 0,1
// 2,3
// 4,5 and so on to 10,11
// even number box clusters are on the left, odd number on the right
// individual boxes within the box clusters start top left as 0 through bottom left as 7 zig zag
//
// Box Cluster # numbers  5,6,7 are super sensative.

#define boxClusterNumber 1

#include "Keyboard.h"
#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include "Color.h"

#define PIN 2
#define BAUD_RATE     (115200)

// DEBUG_SERIAL and the rest must be __false__ for production!
#define DEBUG_SERIAL      (false)
#define DEBUG_PIEZO       (false)
#define DEBUG_HIT_VALUES  (false)

#define randf()    (random(0xffffff) * (1.0f / 0xffffff))

#define ATTRACTOR_FADE_OUT_TIME       (1.6f)
#define ATTRACTOR_HIDE_TIME           (4.5f)
#define ATTRACTOR_FADE_IN_TIME        (3.5f)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_RBG + NEO_KHZ800);

int box[][12] = {      //this fixes all the piezo locations for each box. They were randomly connected and have to be asigned to the correct box
	{3,0,7,9,6,8,2,1}, //box 0
	{1,0,2,8,9,7,3,6},  //box 1

	{9,7,1,0,2,8,6,3},  //box 2
	{9,1,0,2,8,7,3,6},  //box 3

	{0,9,2,1,8,3,7,6},  //box 4
	{6,7,8,2,3,9,1,0},  //box 5

  {3,0,7,1,2,6,8,9},   //box 6
  {1,2,9,3,8,0,6,7},   //box 7

  {7,9,3,1,8,0,2,6},   //box 8
  {2,3,1,6,9,7,0,8},   //box 9

  {8,7,2,3,6,1,9,0},   //box 10
  {9,6,7,1,3,0,2,8},   //box 11
};

// Default hit thresholds
#define HT0     ( 30)
#define HT1     ( 50)
#define HT2     ( 30)
#define HT3     ( 30)
#define HT4     ( 30)
#define HT5     ( 30)
#define HT6     ( 42)
#define HT7     ( 30)
#define HT8     ( 30)
#define HT9     ( 30)
#define HT10    ( 30)
#define HT11    ( 30)

int hitThreshold[][12] = {      //calibration if needed.. default was 20
	{ HT0, HT0, HT0, HT0, HT0, HT0, HT0, HT0}, //box 0
	{ HT1, HT1, HT1, 100, 100, HT1, HT1, HT1},  //box 1
	{ HT2, HT2, HT2, HT2, HT2, 100, 100, 100},  //box 2  - boxes 5,6,7 were extra sensative...
	{ HT3, HT3, HT3, HT3, HT3, HT3, HT3, HT3},  //box 3
	{ HT4, HT4, HT4,  90, HT4, HT4, HT4, HT4},  //box 4
	{  60, HT5, HT5, HT5, HT5, HT5, HT5, HT5},  //box 5
	{ HT6, HT6, HT6, 200, 300, HT6, 100, HT6},  //box 6
	{ HT7, HT7, HT7, HT7, HT7, HT7, HT7, HT7},  //box 7
	{ HT8, HT8, HT8, HT8, 200, 200, HT8, HT8},  //box 8
	{ HT9, HT9, HT9,  50, HT9, HT9, HT9, HT9},  //box 9
	{ 170,HT10, 100,  70, 250,  40,  70,HT10},  //box 10
	{HT11,HT11,HT11,HT11,HT11, 100,  50, 100}  //box 11
};

uint16_t ledArray[]={0,2,4,6,8,10,12,14};

int sensorValue = 0;        // value read from the pot

HSV hsvRandoms[8];
float brights[8] = {0.0f};

long piezoLastHit[8] = {0};
long currentTime = 0;
const int waitBetweenHits = 100;

float elapsed = 0.0f;
float attractor = 1.0f; // Attractor animation fades in/out as appropriate. 0==off, 1==on.
float attractorFadeOut = 0.0f;  // When a piezo is hit: Set this to fade out attractor.

void setup() {

	pinMode(0, OUTPUT);
	digitalWrite(0,LOW);

	pinMode(5,OUTPUT);   //receive / transmit pin... pull high to send
	digitalWrite(5, LOW);

	if (DEBUG_SERIAL || DEBUG_PIEZO || DEBUG_HIT_VALUES) {
		Serial.begin(BAUD_RATE);
	}

	Serial1.begin(BAUD_RATE);

	strip.begin();
	strip.show(); // Initialize all pixels to 'off'
}

void loop() {
	long lastTime = currentTime;
	currentTime = millis();

	float frameSeconds = (currentTime - lastTime) / 1000.0f;
	elapsed += frameSeconds;

	// There is a bug where `elapsed` accumulates so high, that it loses floating
	// point precision, and animations slow down, or freeze entirely :D
	// When elapsed is larger than some large-ish number of seconds: Flip back to 0.
	if (elapsed > (60 * 60 * 3.0f)) {	// OK to reset once every 3 hours?
		elapsed = 0.0f;
	}

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

	if (DEBUG_PIEZO) {
		Serial.println(analogRead(box[boxClusterNumber][7]));
		return;
	}

	// Read values for each piezo sensor
	int hitValues[8] = {0};
	int8_t bestBox = -1;

	for (uint8_t b = 0; b < 8; b++) {
		// Ensure enough time has passed between hits.
		if ((currentTime - piezoLastHit[b]) < waitBetweenHits) continue;

		hitValues[b] = analogRead(box[boxClusterNumber][b]);

		if (DEBUG_HIT_VALUES && (hitValues[b] >= 20)) {
			Serial.print(b);
			Serial.print(":");
			Serial.println(hitValues[b]);
		}

		if (hitValues[b] > hitThreshold[boxClusterNumber][b]) {
			if (bestBox < 0) {
				// This box is the first box with a strong hit.
				bestBox = b;
			} else if (hitValues[b] > hitValues[bestBox]) {
				// This box registered a stronger hit than the leading box.
				bestBox = b;
			}

			// Also: This box is disabled from receiving another hit for 100ms or so.
			piezoLastHit[b] = currentTime;
		}
	}

	if (bestBox >= 0) {
		processHit(bestBox, hitValues[bestBox]);
	}

}

void processHit(uint8_t activeBox, int sensorValue)
{
	// Take control of the data line (RS485 differential pair)
	digitalWrite(5, HIGH);

	Serial1.print(boxClusterNumber + 10);
	Serial1.print(",");
	Serial1.println(activeBox);

	if (DEBUG_SERIAL) {
		Serial.print(boxClusterNumber + 10);
		Serial.print(",");
		Serial.println(activeBox);
		Serial.println(sensorValue);
	}

	// Relinquish control of the data line
	Serial1.flush();

	if (DEBUG_SERIAL) {
		Serial.flush();
	}

	digitalWrite(5, LOW);

	startBoxAnimation(activeBox);
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
