#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <tunet.h>

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

// GUItool: begin automatically generated code
AudioControlSGTL5000     sgtl5000_1;
AudioInputI2S            i2s1;           //xy=223,143
AudioRecordQueue         fluxL;         //xy=632,116
AudioRecordQueue         fluxR;         //xy=650,182
AudioConnection          patchCord1(i2s1, 0, fluxL, 0);
AudioConnection          patchCord2(i2s1, 1, fluxR, 0);

AudioPlayQueue           queue3;         //xy=198,130
AudioPlayQueue           queue4;         //xy=200,266

AudioAnalyzeFFT1024       fft1024_L;       //xy=456,131
AudioAnalyzeFFT1024       fft1024_R;       //xy=476,265

AudioConnection          patchCord3(queue3, fft1024_L);
AudioConnection          patchCord4(queue4, fft1024_R);
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;

char incomingByte = 0;   // for incoming serial data
#define BUFFER_SIZE 4096 // ça fait 16 slides
#define LOCAL_BUFFER 256
#define GLOBAL_THRESHOLD  512
#define FFT_RESOLUTION 512
#define DownSamplingRatio 4
#define DENDRITE_LENGTH 64

char gchrNbElements;

int16_t gblBufferL[BUFFER_SIZE];
int16_t gblBufferR[BUFFER_SIZE];
float gdblMeanR, gdblMeanL, gdblRatioPos, gdlbRatioMax;
int16_t gIntMaxValR, gIntMaxValL, gintMaxPosR, gintMaxPosL;
char gChrNbSlide;
unsigned int guintNbPeak;
TUNet TUPos;
float lfltValues[DENDRITE_LENGTH];

float gfltFFTR[FFT_RESOLUTION], gfltFFTL[FFT_RESOLUTION];
float gfltBuffer4FFTR[LOCAL_BUFFER], gfltBuffer4FFTL[LOCAL_BUFFER];
char gchrDownSamplingPosition;

bool isSDAvailable;
File netFile;

void initNewFile(){
  frecL = SD.open(chrFullNameL, FILE_WRITE);
}

void setup() {
    // Initialize the SD card
    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    isSDAvailable =false;
    if (!(SD.begin(SDCARD_CS_PIN))) {
      // stop here if no SD card, but print a message
      Serial.println("Unable to access the SD card");
      delay(100);
    }
    else
      isSDAvailable = true;//initNewFile();

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
    Serial.println("°_°");
    Serial.println("Welcome to TempUnit hardware interface");
    Serial.println("Type h to display an help message");
    Serial.println("======================================");

    TUPos.setAllNetworkDendriteSize(DENDRITE_LENGTH);
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
                  Serial.println("°_°");
                  Serial.println("TempUnit Hardware Command List:");
                  Serial.println("===============================");
                  Serial.println("h: display this help message");
                  Serial.println("L: display last peak signal from Left sensor");
                  Serial.println("R: display last peak signal from Right sensor");
                  Serial.println("c: display calculated information about last peak.");
                  Serial.println("F: display evaluated information about the Strength of the shock");
                  Serial.println("");
                  Serial.println("a:    Add new TempUnit neuron associated on last peak");
                  Serial.println("l[0]: Learn last peak on TempUnit neuron");
                  Serial.println("-------------------------------------------------------");
                  Serial.println("s[0]: Display score of TempUnit neuron i on last peak");
                  Serial.println("S:    Display output of all TU neurons");
                  Serial.println("n:    Display Network size");
                  Serial.println("m[0]: Display Vector of Mean values");
                  Serial.println("w[0]: Display Weight vector");
                  Serial.println("e[0]: Display std vector");
                  Serial.println("N:    Display the parameters of all the Network");
                  Serial.println("-------------------------------------------------------");
                  Serial.println("P:    Display the number of pools (subnetworks)");
                  Serial.println("p[0]: Display the size of the pool i");
                  Serial.println("-------------------------------------------------------");
                  Serial.println("");
                  Serial.println("-------------------------------------------------------");
                  Serial.println("T[64] : Set the size of the network");
                  Serial.println("t[0]  : Select TempUnit i");
                  Serial.println("d[0]  : Select Synapse position j on dendrite");
                  Serial.println("W[1.0]: Set the Weight of the TempUnit i on the synapse j");
                  Serial.println("E[1.0]: Set the Std on the Neuron i and the synapse j");
                  Serial.println("M[0.0]: Set the value of max response on neuron i, synapse j");
                  Serial.println("");
                  Serial.println("i     : Display the selected neuron i");
                  Serial.println("j     : Display the selected synapse j");
                  Serial.println("");
                  Serial.println("v     : Save current netword to SD card");
                  Serial.println("l     : Load network from SD card");
                  Serial.println("°_°");
                }
                else if (incomingByte==78){ //Display the parameters of all the Network
                    TUPos.showAllPoolParameters();
                }
                else if (incomingByte==83){ // Display output of all TU neurons"
                    TUPos.showAllPoolScore(lfltValues);
                }
                else if (incomingByte==87){//Set the Weight of the TempUnit i on the synapse j
                  unsigned int nb=0;
                  float position=100000;
                  float luinSize = 0;
                  incomingByte = Serial.read();
                  while ((incomingByte>=48)&(incomingByte<=57)) {
                    nb++;
                    luinSize += (incomingByte-48)*position;
                    position /=10;
                    incomingByte = Serial.read();
                    if (nb==6){
                      Serial.println("Error, max value 999'999");
                      break;
                    }
                  }
                  if (nb){
                    luinSize /=pow(10,5-nb+1);
                    if (incomingByte==46){
                      incomingByte = Serial.read();
                      position = 0.1;
                      while ((incomingByte>=48)&(incomingByte<=57)) {
                        luinSize += (incomingByte-48)*position;
                        position /=10;
                        incomingByte = Serial.read();
                      }
                    }

                    Serial.print("Set the weight of the synapse to ");
                    Serial.println(luinSize);
                    TUPos.setWeight(luinSize);
                  }
                  else
                    Serial.println("Cannot determine the desired weight.");
                }
                else if (incomingByte==69){//Set the Std of the TempUnit i on the synapse j
                  unsigned int nb=0;
                  float position=100000;
                  float luinSize = 0;
                  incomingByte = Serial.read();
                  while ((incomingByte>=48)&(incomingByte<=57)) {
                    nb++;
                    luinSize += (incomingByte-48)*position;
                    position /=10;
                    incomingByte = Serial.read();
                    if (nb==6){
                      Serial.println("Error, max value 999'999");
                      break;
                    }
                  }
                  if (nb){
                    luinSize /=pow(10,5-nb+1);
                    if (incomingByte==46){
                      incomingByte = Serial.read();
                      position = 0.1;
                      while ((incomingByte>=48)&(incomingByte<=57)) {
                        luinSize += (incomingByte-48)*position;
                        position /=10;
                        incomingByte = Serial.read();
                      }
                    }

                    Serial.print("Set the std of the synapse to ");
                    Serial.println(luinSize);
                    TUPos.setStd(luinSize);
                  }
                  else
                    Serial.println("Cannot determine the desired std.");
                }
                else if (incomingByte==77){//Set the value of max response on neuron i, synapse j
                  unsigned int nb=0;
                  float position=100000;
                  float luinSize = 0;
                  incomingByte = Serial.read();
                  while ((incomingByte>=48)&(incomingByte<=57)) {
                    nb++;
                    luinSize += (incomingByte-48)*position;
                    position /=10;
                    incomingByte = Serial.read();
                    if (nb==6){
                      Serial.println("Error, max value 999'999");
                      break;
                    }
                  }
                  if (nb){
                    luinSize /=pow(10,5-nb+1);
                    if (incomingByte==46){
                      incomingByte = Serial.read();
                      position = 0.1;
                      while ((incomingByte>=48)&(incomingByte<=57)) {
                        luinSize += (incomingByte-48)*position;
                        position /=10;
                        incomingByte = Serial.read();
                      }
                    }

                    Serial.print("Set the mean of the synapse to ");
                    Serial.println(luinSize);
                    TUPos.setDValue(luinSize);
                  }
                  else
                    Serial.println("Cannot determine the desired mean.");
                }
                else if (incomingByte==84){//T[64] : Set the size of the network
                    unsigned int nb=0;
                    float position=10;
                    unsigned int luinSize = 0;
                    incomingByte = Serial.read();
                    while ((incomingByte>=48)&(incomingByte<=57)) {
                      nb++;
                      Serial.println(nb);
                      luinSize += (incomingByte-48)*position;
                      position /=10;
                      incomingByte = Serial.read();
                    }
                    if (nb){
                      if (nb==1)
                        luinSize /=10;
                      Serial.print("Set the size of the network to ");
                      Serial.println(luinSize);
                      TUPos.setNetSize(luinSize);
                    }
                    else
                      Serial.println("Cannot determine the size of the network.");
                }
                else if (incomingByte==70){
                    Serial.println((gdblMeanR+gdblMeanL)/2.0);
                }
                else if (incomingByte==112){
                  incomingByte = Serial.read();
                  unsigned int toto = incomingByte-48;
                  if (toto>9)
                    toto=0;
                  TUPos.showPoolSize(toto);
                }
                else if (incomingByte==80){
                  TUPos.showPoolsNumber();
                }
                else if (incomingByte==109){
                  incomingByte = Serial.read();
                  Serial.print("Argument :");
                  unsigned int toto = incomingByte-48;
                  Serial.println(toto);
                  if (toto>9)
                    toto=0;
                  TUPos.showDValues(toto);
                }
                else if (incomingByte==119){
                  incomingByte = Serial.read();
                  Serial.print("Argument :");
                  unsigned int toto = incomingByte-48;
                  Serial.println(toto);
                  if (toto>9)
                    toto=0;
                  TUPos.showWeights(toto);
                }
                else if (incomingByte==116){//Select TempUnit i
                  unsigned int nb=0;
                  float position=10;
                  unsigned int luinSize = 0;
                  incomingByte = Serial.read();
                  while ((incomingByte>=48)&(incomingByte<=57)) {
                    nb++;
                    Serial.println(nb);
                    luinSize += (incomingByte-48)*position;
                    position /=10;
                    incomingByte = Serial.read();
                  }
                  if (nb){
                    if (nb==1)
                      luinSize /=10;
                    Serial.print("Select the TempUnit Neuron #");
                    Serial.println(luinSize);
                    TUPos.selectNeuron(luinSize);
                  }
                  else
                    Serial.println("Cannot determine which TempUnit neuron you want");
                }
                else if (incomingByte==100){//Select Synapse j
                  unsigned int nb=0;
                  float position=10;
                  unsigned int luinSize = 0;
                  incomingByte = Serial.read();
                  while ((incomingByte>=48)&(incomingByte<=57)) {
                    nb++;
                    Serial.println(nb);
                    luinSize += (incomingByte-48)*position;
                    position /=10;
                    incomingByte = Serial.read();
                  }
                  if (nb){
                    if (nb==1)
                      luinSize /=10;
                    Serial.print("Select the Synapse #");
                    Serial.println(luinSize);
                    TUPos.selectSynapse(luinSize);
                  }
                  else
                    Serial.println("Cannot determine which Synapse you want");
                }
                else if (incomingByte==101){
                  incomingByte = Serial.read();
                  Serial.print("Argument :");
                  unsigned int toto = incomingByte-48;
                  Serial.println(toto);
                  if (toto>9)
                    toto=0;
                  TUPos.showStd(toto);
                }
                else if (incomingByte==110){
                  Serial.println(TUPos.getTUNetSize());
                }
                else if (incomingByte==97){// Add new TempUnit neuron associated on last peak
                  if (guintNbPeak){
                    TUPos.setNewTU(lfltValues);
                  }
                }
                else if (incomingByte==108){//Learn, Adapt to last peak
                    incomingByte = Serial.read();
                    Serial.print("Argument :");
                    unsigned int toto = incomingByte-48;
                    Serial.println(toto);
                    if (toto>9)
                      toto=0;
                    TUPos.learnNewVector(toto,lfltValues);
                }
                else if (incomingByte==105){
                  TUPos.showSelectedNeuron();
                }
                else if (incomingByte==106){
                  TUPos.showSelectedSynapse();
                }
                else if (incomingByte==118){ //Save current netword to SD card

                }
                else if (incomingByte==108){ //Load network from SD card


                }
                else if (incomingByte==115){//get TempUnit score on last peak
                    incomingByte = Serial.read();
                    Serial.print("Argument :");
                    unsigned int toto = incomingByte-48;
                    Serial.println(toto);
                    if (toto>9)
                      toto=0;
                    TUPos.showIndividualScore(toto,lfltValues);
                }
                else if (incomingByte==76){
                  Serial.println("Here is the Left peak ! ");
                  for (int i=0;i<BUFFER_SIZE;i++){
                    Serial.println(gblBufferL[i]);
                  }
                }
                else if (incomingByte==99){
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

                gchrDownSamplingPosition = 0;
                //Should start the FFT immediately

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
            for (int i=0;i<FFT_RESOLUTION;i++){
              gfltFFTL[i] = 0;
              gfltFFTR[i] = 0;
            }
            for (int i=0;i<gchrNbElements;i++){
              memcpy(queue3.getBuffer(), gblBufferL+(LOCAL_BUFFER/2)*i, LOCAL_BUFFER);
              memcpy(queue4.getBuffer(), gblBufferR+(LOCAL_BUFFER/2)*i, LOCAL_BUFFER);
              queue3.playBuffer();
              queue4.playBuffer();
              delay(12);
              if (fft1024_L.available()){
                for (int j=0;j<FFT_RESOLUTION;j++)
                  gfltFFTL[j] += fft1024_L.read(j);
              }
              if (fft1024_R.available()){
                for (int j=0;j<FFT_RESOLUTION;j++)
                  gfltFFTR[j] += fft1024_R.read(j);
              }
            }
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

            lfltValues[0] = gdblMeanL;
            lfltValues[1] = gdblMeanR;
            lfltValues[2] = gdblMeanR/gdblMeanL;
            lfltValues[3] = gIntMaxValL;
            lfltValues[4] = gIntMaxValR;
            lfltValues[5] = gdlbRatioMax;
            lfltValues[6] = gintMaxPosL;
            lfltValues[7] = gintMaxPosR;
            lfltValues[8] = gdblRatioPos;
            lfltValues[9] = (gdblMeanR+gdblMeanL)/2.0;
            {
              int k=10;
              for (int j=0;j<27;j++){
                lfltValues[k] = gfltFFTL[j];
                k++;
                lfltValues[k] = gfltFFTR[j];
                k++;
              }
              Serial.println(TUPos.getWinnerID(lfltValues));
            }
           }
          if ((gChrNbSlide<(gchrNbElements+1))&&(gChrNbSlide>0)){
            memcpy(gblBufferL+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferL, LOCAL_BUFFER);
            memcpy(gblBufferR+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferR, LOCAL_BUFFER);
          }
        }
}
