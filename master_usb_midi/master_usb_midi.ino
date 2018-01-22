
/*
TEENSY: Set USB type to Serial
*/

#include "PitchToNote.h"

const int MAX_LEN = 10;
const char lineEnding = '\n'; // whatever marks the end of your input.
char inputSentence [MAX_LEN + 1];
int inputIndex;
bool newInput;

const byte MAX_TOKENS = 3;
const char* delimiters = ", "; // whatever characters delimit your input string
char* tokens [MAX_TOKENS + 1];
enum indexName {box, pixel};
#define PRINT_ITEM(x) printItem (x, #x)


const byte notePitches[][8] = {  //some test notes
  { 0,  1,  2,  3,  4,  5,  6,  7},
  { 8,  9,  10, 11, 12, 13, 14, 15},
  { 16, 17, 18, 19, 20, 21, 22, 23},
  { 24, 25, 26, 27, 28, 29, 30, 31},
  { 32, 33, 34, 35, 36, 37, 38, 39},
  { 40, 41, 42, 43, 44, 45, 46, 47},
  { 48, 49, 50, 51, 52, 53, 54, 55},
  { 56, 57, 58, 59, 60, 61, 62, 63}
};


int inByte = 0;
int lastInByte = 0;

void setup() {

  pinMode(5,OUTPUT);   //receive / transmit pin... pull low to receive for Master
  digitalWrite(5, LOW);

  // initialize both serial ports:
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop() {

  serialEvent(); //RS485 incoming

  if (newInput && strlen (inputSentence))
  {
    int tokenIndex = 0;
    //Serial.println (inputSentence); // tell 'em what you've got
    tokens [tokenIndex] = strtok (inputSentence, delimiters);
    while ((tokenIndex < MAX_TOKENS - 1) && tokens [tokenIndex])
    {
      tokenIndex++;
      tokens [tokenIndex] = strtok (NULL, delimiters);
    }

    PRINT_ITEM (box);
    PRINT_ITEM (pixel);


    int boxNumber = atoi(tokens[0]) - 10;
    int pixelNumber = atoi(tokens[1]);
    byte pitch = notePitches[boxNumber][pixelNumber];

    Serial.println(boxNumber);
    Serial.println(pixelNumber);
    Serial.println(pitch);

    usbMIDI.sendNoteOn(pitch, 99, 1);  // 60 = C4

   // reset things for the next lot.
   newInput = false;
   inputIndex = 0;
   inputSentence [0] = '\0';
 }
}



void serialEvent ()  // build the input string.
{
 while (Serial1.available() ) //read from hardware serial
 {
   char readChar = Serial1.read ();
   if (readChar == lineEnding)
   {
     newInput = true;
   }
   else
   {
     if (inputIndex < MAX_LEN)
     {
       inputSentence [inputIndex++] = readChar;
       inputSentence [inputIndex] = '\0';
     }
   }
 }
}

void printItem (int index, char* name)
{
 Serial.print (name);
 Serial.print (F(" "));
 Serial.println (tokens [index]);
}



