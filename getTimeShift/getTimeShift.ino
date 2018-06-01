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

#define GLOBAL_THRESHOLD  16
#define MAX_TIMESHIFT 25 // shift in ms
#define MIN_ISI 200//InterSpikeInterval 100 // shift in ms

const int myInput = AUDIO_INPUT_LINEIN;
//Permet de stocker le temps en ms et savoir où on en est.
unsigned long ulngTime, UlngLastActivated;
bool gblActivated, gblLeftActivated, gblRightActivated;

unsigned int guintCounter =0; 

void setup() {
  AudioMemory(60);
  gblActivated = false;
  //Enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.9);

  fluxL.begin();
  fluxR.begin();
  Serial.begin(115200);
}

void loop(){
  int16_t bufferL[128];
  int16_t bufferR[128];
  int lIntBufferSize = fluxL.available();
  
  if (lIntBufferSize>= 1){
    memcpy(bufferL, fluxL.readBuffer(), 128);
    fluxL.freeBuffer();
    memcpy(bufferR, fluxR.readBuffer(), 128);
    fluxR.freeBuffer();

    for (int i=0;i<lIntBufferSize;i++)
    {
      guintCounter++;
      if ((bufferR[i]>GLOBAL_THRESHOLD)||(bufferR[i]<GLOBAL_THRESHOLD*-1)){
        if ((!gblActivated)&&(millis()-ulngTime)>MIN_ISI){
          ulngTime = millis();
          gblActivated = true;
          digitalWrite(13,HIGH);
          gblRightActivated = true;
          guintCounter = 0;
        }
        else if (gblLeftActivated){
          gblActivated = false;
          //Serial.println(millis()-ulngTime);
          Serial.println(guintCounter);
          digitalWrite(13,LOW);
          gblLeftActivated = false;
        }       
      }
      if (bufferL[i]>GLOBAL_THRESHOLD){    
        if ((!gblActivated)&&(millis()-ulngTime)>MIN_ISI){
          ulngTime = millis();
          gblActivated = true;
          digitalWrite(13,HIGH);
          gblLeftActivated = true;
          guintCounter = 0;
        }
        else if (gblRightActivated){
          gblActivated = false;
          Serial.print("-");
          //Serial.println(millis()-ulngTime);
          Serial.println(guintCounter);
          digitalWrite(13,LOW);
          gblRightActivated = false;
        }
      }
      if ((gblActivated)&&(millis()-ulngTime)>MAX_TIMESHIFT){
        gblActivated=false; 
        gblRightActivated = false;
        gblLeftActivated = false; 
        ulngTime = millis();
        guintCounter = 0;
      }
    }
  } 
}

