#include <stdint.h>

#define lerp(a,b,v)    ((a) + ((b) - (a)) * (v))

/*
const uint8_t PROGMEM gamma8[] = {
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
		1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
		2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
		5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	 25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	 51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	 69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	 90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
*/

const uint8_t PROGMEM gamma8[] = {
  0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
  1,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   4,   4,
  4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,
  8,   8,   9,   9,   9,  10,  10,  10,  11,  11,  12,  12,  12,  13,  13,  14,
 14,  15,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  22,
 22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,  30,  30,  31,  32,
 33,  33,  34,  35,  36,  36,  37,  38,  39,  40,  40,  41,  42,  43,  44,  45,
 46,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
 61,  62,  63,  64,  65,  67,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,
 80,  81,  82,  83,  85,  86,  87,  89,  90,  91,  93,  94,  95,  97,  98,  99,
101, 102, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119, 121, 122, 124,
125, 127, 129, 130, 132, 134, 135, 137, 139, 141, 142, 144, 146, 148, 150, 151,
153, 155, 157, 159, 161, 163, 165, 166, 168, 170, 172, 174, 176, 178, 180, 182,
184, 186, 189, 191, 193, 195, 197, 199, 201, 204, 206, 208, 210, 212, 215, 217,
219, 221, 224, 226, 228, 231, 233, 235, 238, 240, 243, 245, 248, 250, 253, 255 };

// RGB: Range of each channel is 0.0...1.0
struct RGB {
  float r;  // red
  float g;  // green
  float b;  // blue
};

struct RGB8 {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

// Ranges: 0.0...1.0
struct HSV {
  float h;  // hue. Range: 0..1
  float s;  // saturation
  float v;  // value (brightness)
};

RGB hsv2rgb( float hF, float sF, float vF ) {
  // Expand H by 6 times for the 6 color regions. Wrap the remainder to 0..1
  float hWrap = (hF*6.0);
  hWrap = hWrap - floor(hWrap);  // floating point remainder. 0..1 for each of the 6 color regions

  float v = vF;  // top (max) value
  float p = vF * (1.0-sF);  // bottom (min) value
  float q = vF * (1.0-(hWrap*sF));  // falling (yellow->green: the red channel falls)
  float t = vF * (1.0 - ((1.0-hWrap)*sF));  // rising (red->yellow: the green channel rises)

  // Need to find the correct color region that the hue belongs to.
  // Hue can have a negative value, so need to step it to positive space, so the modulus % operator behaves as expected.
  float hF_pos = hF;
  if( hF_pos < 0.0 ) {
    hF_pos += ceil(-hF_pos);
  }
  uint8_t hue_i = (uint8_t)(floor( hF_pos * 6.0 )) % 6;

  switch( hue_i ) {
    case 0:  return (RGB){v,t,p};  // red -> yellow
    case 1:  return (RGB){q,v,p};  // yellow -> green
    case 2:  return (RGB){p,v,t};  // green -> cyan
    case 3:  return (RGB){p,q,v};  // cyan -> blue
    case 4:  return (RGB){t,p,v};  // blue -> magenta
    case 5:  return (RGB){v,p,q};  // magenta -> red
  }

  return (RGB){0,0,0};  // sanity escape
};

RGB boxColorActive(uint8_t cluster, HSV * rando, float bright, float cycle)
{
	switch (cluster) {

		case 0:
		{
			// Color & white exchange
			float wave = (sin(bright * rando->s * 60.0f + 190.0f) + 1.0f) * 0.5f;

			return hsv2rgb(
				rando->h * 0.1f + (0.6667f - 0.05f) - wave * 0.1f + cycle / 120.0f + sin(cycle / 10.0f) * 0.5f,
				lerp(1.0f, 0.3f, wave),
				bright
			);
		}
		break;

		case 1:
		{
			// Test
			return (RGB){.r = 0, .g = 0, .b = bright};
		}
		break;

		case 2:
		{
			// White -> fade to pastels
			return hsv2rgb(rando->h, 1.0 - bright, bright);
		}
		break;

		case 3:
		{
			// Red/orange/yellow fire. Hue is kinda perlin noise, sum of harmonics.
			const float AMP0 = 1.0f;
			const float AMP1 = 0.7f;
			const float AMP2 = 0.5f;
			float p = bright * 200.0f;
			float n = sin(rando->h * p) * AMP0 +
			          sin(rando->s * p) * AMP1 +
			          sin(rando->v * p) * AMP2;
			n = (n * (0.5f / (AMP0 + AMP1 + AMP2))) + 0.5f;

			float speedUp = lerp(1.0, 3.0f, rando->h);
			float fastFade = max(0.0f, bright * (speedUp + 1.0f) - speedUp);
			float decay = lerp(fastFade, fastFade * fastFade, 0.5f + rando->v * 0.5f);

			return hsv2rgb(n * 0.16667f, 1.0f, decay);
		}
		break;

		case 4:
		{
			// Rainbow-
			return hsv2rgb(rando->h + bright, 1.0, bright);
		}
		break;

		case 5:
		{
			// 80's sweater: Lime green, cyan, dark blue, magenta
			float speedUp = lerp(0.65, 0.85f, rando->s);
			float fastFade = max(0.0f, bright * (speedUp + 1.0f) - speedUp);

			float h = rando->h * 0.5f + 0.36f;

			// Let's try a weird attack: Start at ~0.5 brightness, then decay.
			float b2 = fastFade * fastFade;
			float b4 = b2 * b2;

			// I want some warble too
			float warble = (cos((rando->v + 0.9f) * (1.0f - bright) * 80.0f) + 1.0f) * 0.5f;
			float v = lerp(fastFade, warble, 0.12f);

			return hsv2rgb(h + b4 * 0.075f, 1.0f, v * fastFade);
		}
		break;

		default: break;
	}

	return (RGB){0.0f, 0.0f, 0.0f};
}

// Yes, applies gamma correction
RGB8 rgbToRGB8(RGB rgb) {
	float r = constrain(rgb.r, 0.0f, 1.0f);
	float g = constrain(rgb.g, 0.0f, 1.0f);
	float b = constrain(rgb.b, 0.0f, 1.0f);

	uint8_t rx = (uint8_t)(r * 0xff);
	uint8_t gx = (uint8_t)(g * 0xff);
	uint8_t bx = (uint8_t)(b * 0xff);

	return (RGB8){
		.r = pgm_read_byte(&gamma8[rx]),
		.g = pgm_read_byte(&gamma8[gx]),
		.b = pgm_read_byte(&gamma8[bx])
	};
}
