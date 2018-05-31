#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

// GUItool: begin automatically generated code
AudioControlSGTL5000     sgtl5000_1;
AudioInputI2S            i2s1;           //xy=223,143
AudioRecordQueue         fluxL;         //xy=632,116
AudioRecordQueue         fluxR;         //xy=650,182
AudioConnection          patchCord1(i2s1, 0, fluxL, 0);
AudioConnection          patchCord2(i2s1, 1, fluxR, 0);
// GUItool: end automatically generated code

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_LINEIN;
//Permet de stocker le temps en ms et savoir où on en est.
unsigned long ulngTime;

void setup() {
  AudioMemory(52);
  pinMode(13, OUTPUT);

  //Enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);

  fluxL.begin(); // I forgot that shit !
  fluxR.begin();
}

void loop(){
  int16_t bufferL[128];
  int16_t bufferR[128];
  int lIntBufferSize = 0;
  lIntBufferSize = fluxL.available();
  memcpy(bufferL, fluxL.readBuffer(), lIntBufferSize);
  memcpy(bufferR, fluxR.readBuffer(), lIntBufferSize);
  fluxL.freeBuffer();
  fluxR.freeBuffer();
 
  for (int i=0;i<lIntBufferSize;i++){
    if (bufferR[i]>20000)
      Serial.println(bufferR[i]);
    //delay(1);
  }
}

