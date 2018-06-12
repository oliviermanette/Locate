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
#define HIGH_THRESHOLD 2048
#define LOW_THRESHOLD 512
#define MAX_TIMESHIFT 25 // shift in ms
#define MIN_ISI 200//InterSpikeInterval 100 // shift in ms
#define BUFFER_SIZE 128

const int myInput = AUDIO_INPUT_LINEIN;
//Permet de stocker le temps en ms et savoir où on en est.
unsigned long ulngTime, UlngLastActivated;
bool gblActivated, gblLeftActivated, gblRightActivated;

unsigned int guintCounter =0, guintEcart=0; 

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
  int16_t lintSeuil =0;
  bool lblRightDetected = false, lblLeftDetected = false;
  gblLeftActivated = false;
  gblRightActivated = false;
  
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
    lintSeuil = HIGH_THRESHOLD;
    for (int i=0;i<BUFFER_SIZE;i++){
      guintCounter++;
      if ((bufferR[i]>lintSeuil)&& (gblLeftActivated)){ //2eme pic à droite le gauche est détecté
        guintEcart = guintCounter;
        gblRightActivated = true;
        lintMaxR = bufferR[i];
        guintCounter = 0;
      }
      else if ((bufferR[i]>lintSeuil) && (gblRightActivated == false)){ // 1er pic à droite détecté en premier
      
        lintMaxR = bufferR[i];
        lintSeuil = LOW_THRESHOLD;
        gblRightActivated = true;
        guintCounter = 0;
      }
      else if ((bufferL[i]>lintSeuil)&& (gblRightActivated)){ // pic à gauche, le droit est détecté
        guintEcart = guintCounter;
        lintMaxL = bufferL[i];
        gblLeftActivated = true;
        lintSeuil = HIGH_THRESHOLD;
        guintCounter = 0;
      }
      else if ((bufferL[i]>lintSeuil)&& (gblLeftActivated == false)){ // pic à gauche détecté en premier
        lintMaxL = bufferL[i];
        gblLeftActivated = true;
        lintSeuil = LOW_THRESHOLD;
        guintCounter = 0;
      }
    }
    //Serial.print("Max = ");
    if ((gblRightActivated == gblLeftActivated)&&(gblLeftActivated)){
      Serial.println("Pic transmis détecté !");
      Serial.print("Ecart : ");
      Serial.println(guintEcart);
      Serial.print("Max Right : ");
      Serial.println(lintMaxR);
      Serial.print("Max Left : ");
      Serial.println(lintMaxL);
    }
 
  }    
} 

