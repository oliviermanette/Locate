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

const int myInput = AUDIO_INPUT_LINEIN;
//Permet de stocker le temps en ms et savoir où on en est.
unsigned long ulngTime;

void setup() {
  AudioMemory(60);

  //Enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);

  fluxL.begin();
  fluxR.begin();
  Serial.begin(115200);
}

void loop(){
  int16_t bufferL[128];
  int16_t bufferR[128];
  int lIntBufferSize = fluxL.available();
  digitalWrite(13,LOW);
  
  if (lIntBufferSize>= 2){
    digitalWrite(13,HIGH);
    memcpy(bufferL, fluxL.readBuffer(), lIntBufferSize);
    fluxL.freeBuffer();
    memcpy(bufferR, fluxR.readBuffer(), lIntBufferSize);
    fluxR.freeBuffer();

    for (int i=0;i<lIntBufferSize;i++){
      //if (bufferR[i]>64)
        Serial.println(bufferL[i]-bufferR[i]);
      //Serial.println(bufferR[i]);
    }
  } 
}

