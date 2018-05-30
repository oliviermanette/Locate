#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <TimeLib.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=223,143
AudioRecordQueue         fileL;         //xy=632,116
AudioRecordQueue         fileR;         //xy=650,182
AudioConnection          patchCord1(i2s1, 0, fileL, 0);
AudioConnection          patchCord2(i2s1, 1, fileR, 0);
// GUItool: end automatically generated code
AudioControlSGTL5000     sgtl5000_1;

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_LINEIN;
//Permet de stocker le temps en ms et savoir où on en est.
unsigned long ulngTime;

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

// The file where data is recorded
//File frec,frecL;

// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playin

void setup() {
  AudioMemory(60);

  //Enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);

}

time_t getTeensy3Time(){
  return Teensy3Clock.get();
}

void loop(){
  byte bufferL[256];
  byte bufferR[256];
  int intValeur = 0;
  // Fetch 2 blocks from the audio library and copy
  // into a 512 byte buffer.  The Arduino SD library
  // is most efficient when full 512 byte sector size
  // writes are used.
  memcpy(bufferL, fileL.readBuffer(), 256);
  fileL.freeBuffer();
  memcpy(bufferR, fileR.readBuffer(), 256);
  fileR.freeBuffer();

  
  for (int i=0;i<256;i+=2){
    intValeur = BitShiftCombine(bufferL[i],bufferL[i+1]);
   // if (intValeur>16000){
      Serial.println(intValeur);
   // }
    intValeur = BitShiftCombine(bufferR[i],bufferR[i+1]);
   // if (BitShiftCombine(bufferR[i+1],bufferR[i])>16000){
      Serial.println(intValeur);
   // }
  }
}

int BitShiftCombine( byte x_high, byte x_low)
{
  int combined; 
  combined = x_high;              //send x_high to rightmost 8 bits
  combined = combined<<8;         //shift x_high over to leftmost 8 bits
  combined |= x_low;                 //logical OR keeps x_high intact in combined and fills in
  //rightmost 8 bits
  return combined;
}

