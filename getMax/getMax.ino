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

#define GLOBAL_THRESHOLD  512
#define MAX_TIMESHIFT 25 // shift in ms
#define MIN_ISI 200//InterSpikeInterval 100 // shift in ms
#define BUFFER_SIZE 128

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
  sgtl5000_1.volume(1);

  fluxL.begin();
  fluxR.begin();
  Serial.begin(115200);
}

void loop(){
  int16_t bufferL[BUFFER_SIZE];
  int16_t bufferR[BUFFER_SIZE];
  
  for (int i=0;i<BUFFER_SIZE;i++){
    bufferR[i]=0;
    bufferL[i]=0;
  }
  int16_t lintMaxL=0, lintMaxR =0;
  int lIntBufferSize = fluxR.available();
  
  if (lIntBufferSize>= 1){
    memcpy(bufferL, fluxL.readBuffer(), BUFFER_SIZE);
    fluxL.freeBuffer();
    memcpy(bufferR, fluxR.readBuffer(), BUFFER_SIZE);
    fluxR.freeBuffer();
    
    for (int i=0;i<BUFFER_SIZE;i++){
      guintCounter++;
      if (bufferR[i]>lintMaxR){
      
        lintMaxR = bufferR[i];
      }
      if (bufferL[i]>lintMaxL){
        lintMaxL = bufferL[i];
      }
         //Serial.println(bufferR[i]);
        //delayMicroseconds(50);
    }
    //Serial.print("Max = ");
    if (lintMaxR>GLOBAL_THRESHOLD)
      Serial.println(lintMaxL);
 
  }    
} 

