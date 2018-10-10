#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <tempunit.h>

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
float gdblMeanR, gdblMeanL, gdblRatioPos, gdlbRatioMax;
int16_t gIntMaxValR, gIntMaxValL, gintMaxPosR, gintMaxPosL;
char gChrNbSlide;
unsigned int guintNbPeak;
TempUnit TUPos;
float lfltValues[9];

void setup() {
    gchrNbElements = 2*BUFFER_SIZE/LOCAL_BUFFER;
    gdblMeanR = 0;
    gIntMaxValR= 0;
    gIntMaxValL= 0;
    gintMaxPosR= 0;
    gintMaxPosL= 0;
    gdblMeanL = 0;
    gdblRatioPos= 0;
    gdlbRatioMax= 0;
    guintNbPeak = 0;
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

    TUPos.setDendriteSize(9);
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
                else if (incomingByte==104){
                  //Display Help message
                  Serial.println("");
                  Serial.println("Command List:");
                  Serial.println("=============");
                  Serial.println("h: display this help message");
                  Serial.println("L: display last peak signal from Left sensor");
                  Serial.println("R: display last peak signal from Right sensor");
                  Serial.println("M: display calculated information about last peak.");
                  Serial.println("");
                  Serial.println("a: Add new TempUnit neuron associated on last peak");
                  Serial.println("l: Learn last peak on TempUnit neuron");
                  Serial.println("g: get score of TempUnit neuron on last peak");
                  Serial.println("");
                }
                else if (incomingByte==97){// Add new TempUnit neuron associated on last peak
                  if (guintNbPeak){
                    
                    lfltValues[0] = gdblMeanL;
                    lfltValues[1] = gdblMeanR;
                    lfltValues[2] = gdblMeanR/gdblMeanL;
                    lfltValues[3] = gIntMaxValL;
                    lfltValues[4] = gIntMaxValR;
                    lfltValues[5] = gdlbRatioMax;
                    lfltValues[6] = gintMaxPosL;
                    lfltValues[7] = gintMaxPosR;
                    lfltValues[8] = gdblRatioPos;
                    TUPos.setNewTU(lfltValues);
                  }
                  
                }
                else if (incomingByte==108){//Learn, Adapt to last peak
                    lfltValues[0] = gdblMeanL;
                    lfltValues[1] = gdblMeanR;
                    lfltValues[2] = gdblMeanR/gdblMeanL;
                    lfltValues[3] = gIntMaxValL;
                    lfltValues[4] = gIntMaxValR;
                    lfltValues[5] = gdlbRatioMax;
                    lfltValues[6] = gintMaxPosL;
                    lfltValues[7] = gintMaxPosR;
                    lfltValues[8] = gdblRatioPos;
                    TUPos.learnNewVector(lfltValues);
                }
                else if (incomingByte==103){//get TempUnit score on last peak
                    lfltValues[0] = gdblMeanL;
                    lfltValues[1] = gdblMeanR;
                    lfltValues[2] = gdblMeanR/gdblMeanL;
                    lfltValues[3] = gIntMaxValL;
                    lfltValues[4] = gIntMaxValR;
                    lfltValues[5] = gdlbRatioMax;
                    lfltValues[6] = gintMaxPosL;
                    lfltValues[7] = gintMaxPosR;
                    lfltValues[8] = gdblRatioPos;
                    TUPos.getScore(lfltValues);
                }
                else if (incomingByte==76){
                  Serial.println("Here is the Left peak ! ");
                  for (int i=0;i<BUFFER_SIZE;i++){
                    Serial.println(gblBufferL[i]);
                  }
                }
                else if (incomingByte==77){
                  Serial.print("Here is the Left peak Mean : ");
                  Serial.println(gdblMeanL);
                  Serial.print("Here is the Right peak Mean : ");
                  Serial.println(gdblMeanR);
                  Serial.print("Here is the Ratio Mean : ");
                  Serial.println(gdblMeanR/gdblMeanL);

                  Serial.print("Here is the Left peak Max Value : ");
                  Serial.println(gIntMaxValL);
                  Serial.print("Here is the Right peak Max Value : ");
                  Serial.println(gIntMaxValR);
                  Serial.print("Here is the Ratio Max Value : ");
                  Serial.println(gdlbRatioMax);
 
                  Serial.print("Here is the Left peak Max Position : ");
                  Serial.println(gintMaxPosL);
                  Serial.print("Here is the Right peak Max Positon : ");
                  Serial.println(gintMaxPosR);
                  Serial.print("Here is the Ratio Mean : ");
                  Serial.println(gdblRatioPos);
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
                guintNbPeak++;
                // Reset values for next peak
                gdblMeanR = 0;
                gdblMeanL = 0;
                gIntMaxValR= 0;
                gIntMaxValL= 0;
                gintMaxPosR= 0;
                gintMaxPosL= 0;
                gdblRatioPos= 0;
                gdlbRatioMax= 0;
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
            //C'est fini, on peut travailler sur les données
            int16_t lintTmpValue;
            for (int i=0;i<BUFFER_SIZE;i++){
              if (gblBufferR[i]>=0)
                lintTmpValue = gblBufferR[i];
              else
                lintTmpValue = -1 * gblBufferR[i];
              gdblMeanR += lintTmpValue;

              if (lintTmpValue>gIntMaxValR){
                gIntMaxValR = lintTmpValue;
                gintMaxPosR = i;
              }
              
              if (gblBufferL[i]>=0)
                lintTmpValue = gblBufferL[i];
              else
                lintTmpValue = -1 * gblBufferL[i];
              if (lintTmpValue>gIntMaxValL){
                gIntMaxValL = lintTmpValue;
                gintMaxPosL = i;
              }
              
              gdblMeanL += lintTmpValue;
            }
            gdblMeanL /=BUFFER_SIZE;
            gdblMeanR /=BUFFER_SIZE;
            gdblRatioPos= (double)gintMaxPosL/(double)gintMaxPosR;
            gdlbRatioMax= (double)gIntMaxValR/(double)gIntMaxValL;
           }
     
             
          if ((gChrNbSlide<(gchrNbElements+1))&&(gChrNbSlide>0)){
            memcpy(gblBufferL+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferL, LOCAL_BUFFER);
            memcpy(gblBufferR+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferR, LOCAL_BUFFER);
          }
        }
}
