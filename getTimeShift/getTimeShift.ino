#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <TimeLib.h>

// GUItool: begin automatically generated code
AudioControlSGTL5000     sgtl5000_1;
AudioInputI2S            i2s1;           //xy=223,143
AudioRecordQueue         fileL;         //xy=632,116
AudioRecordQueue         fileR;         //xy=650,182
AudioConnection          patchCord1(i2s1, 0, fileL, 0);
AudioConnection          patchCord2(i2s1, 1, fileR, 0);
// GUItool: end automatically generated code

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_LINEIN;
//Permet de stocker le temps en ms et savoir où on en est.
unsigned long ulngTime;

void setup() {
  AudioMemory(60);
  pinMode(13, OUTPUT);

  //Enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
}

void loop(){
  int16_t bufferL[128];
  int16_t bufferR[128];

  memcpy(bufferL, fileL.readBuffer(), 128);
  fileL.freeBuffer();
  memcpy(bufferR, fileR.readBuffer(), 128);
  fileR.freeBuffer();
 
  for (int i=0;i<128;i++){
    Serial.println(bufferR[i]);
    delay(1);
  }
}

