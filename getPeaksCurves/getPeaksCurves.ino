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

const int myInput = AUDIO_INPUT_LINEIN;

int incomingByte = 0;   // for incoming serial data
#define BUFFER_SIZE 4096 // ça fait 16 slides
#define LOCAL_BUFFER 256
#define GLOBAL_THRESHOLD  512

char gchrNbElements;

int16_t gblBufferL[BUFFER_SIZE];
int16_t gblBufferR[BUFFER_SIZE];
char gChrNbSlide;

void setup() {
    gchrNbElements = 2*BUFFER_SIZE/LOCAL_BUFFER;
    AudioMemory(256);
    //Enable the audio shield
    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(myInput);
    sgtl5000_1.volume(1);
    gChrNbSlide = 0;

    for (int i=0;i<BUFFER_SIZE;i++){
        gblBufferR[i]=0;
        gblBufferL[i]=0;
    }
    fluxL.begin();
    fluxR.begin();
    Serial.begin(115200);
}

void loop() {
        int16_t bufferL[LOCAL_BUFFER];
        int16_t bufferR[LOCAL_BUFFER];
        for (int i=0;i<LOCAL_BUFFER;i++){
            bufferL[i]=0;
            bufferR[i]=0;
        }
        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();

                if (incomingByte==82){
                  Serial.println("Here is the Right peak ! ");
                  for (int i=0;i<BUFFER_SIZE;i++){
                    Serial.println(gblBufferR[i]);
                  }
                }
                else if (incomingByte==76){
                  Serial.println("Here is the Left peak ! ");
                  for (int i=0;i<BUFFER_SIZE;i++){
                    Serial.println(gblBufferL[i]);
                  }
                }           
        }
        int lIntBufferSize = fluxR.available();
        if (lIntBufferSize>= 1){

          memcpy(bufferL, fluxL.readBuffer(), LOCAL_BUFFER);
          memcpy(bufferR, fluxR.readBuffer(), LOCAL_BUFFER);
          fluxL.freeBuffer();
          fluxR.freeBuffer();

          if (gChrNbSlide==0){
            for (int i=0;i<LOCAL_BUFFER;i++){
              if ((bufferR[i]>GLOBAL_THRESHOLD)||
                  (bufferR[i]<(-1*GLOBAL_THRESHOLD))||
                  (bufferL[i]>GLOBAL_THRESHOLD)||
                  (bufferL[i]<(-1*GLOBAL_THRESHOLD))){
                //Sauvegarde
                gChrNbSlide = 1;
                Serial.println("Peak détecté");
                break;
              }
            }
          }
          else if (gChrNbSlide<(gchrNbElements+1)){
            //Ajoute
            gChrNbSlide++;
          }
          else if (gChrNbSlide==(gchrNbElements+1)){
            gChrNbSlide = 0;
          }
             
          if ((gChrNbSlide<(gchrNbElements+1))&&(gChrNbSlide>0)){
            memcpy(gblBufferL+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferL, LOCAL_BUFFER);
            memcpy(gblBufferR+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferR, LOCAL_BUFFER);
          }
        }
}
