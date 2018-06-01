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
File frec,frecL;

// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playin

void setup() {
  AudioMemory(60);

  //Enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    Serial.println("Unable to access the SD card");
    delay(500);
  }
  initNewFile();
}

time_t getTeensy3Time(){
  return Teensy3Clock.get();
}

void initNewFile()
{
  //Récupère les information de temps et les affiches
  setSyncProvider(getTeensy3Time);
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 

  // Crée un nom de fichier et de dossier qui dépend du jour
  // et de la date
  char chrFolderName[9];
  String strFolderName = "";
  String strFullFilename="";
  char chrFullNameL[24];
  strFolderName += year();
  strFolderName += month();
  strFolderName += day();
  strFolderName.toCharArray(chrFolderName,9);
  if (!SD.exists(chrFolderName)){
    Serial.print("Create folder : ");
    Serial.println(chrFolderName);
    Serial.println(strFolderName);
    SD.mkdir(chrFolderName);  
  }
  else{
    Serial.println("Folder already exists, skip.");
    Serial.println("");
  }
    strFullFilename = strFolderName+ "/" + hour() + "_" + minute() + "_" + second() + ".dat";
    strFullFilename.toCharArray(chrFullNameL,24);
    Serial.println(chrFullNameL);
    if (SD.exists(chrFullNameL)) {
      SD.remove(chrFullNameL);
      Serial.print("This file already exists, it will be replaced by new data : ");
      Serial.println(chrFullNameL);
    }
    frecL = SD.open(chrFullNameL, FILE_WRITE);
    Serial.println("");
    Serial.println("");
    //Serial.println(frecL);
    if (frecL) {
      mode = 1;
      Serial.print("Recording ");
      Serial.println(chrFullNameL);    
    }
    else{
      mode = 0;
      Serial.println("The file cannot be created, have to stop recording");
    }
    if (mode==1){
      strFullFilename = strFolderName+ "/" + hour() + "_" + minute() + "r" + second() + ".dat";
      strFullFilename.toCharArray(chrFullNameL,24);
      Serial.println(chrFullNameL);
      //Serial.println(strFullFilename.length());
      
      if (SD.exists(chrFullNameL)) {
          SD.remove(chrFullNameL);
          Serial.print("This file already exists, it will be replaced by new data : ");
          Serial.println(chrFullNameL);
      }
      frec = SD.open(chrFullNameL, FILE_WRITE);
      //Serial.println(frec);
      if (frec) {
          fileL.begin();
          fileR.begin();
          ulngTime = millis();
          mode = 1;
          Serial.print("Recording ");
          Serial.println(chrFullNameL);
          // 2 : Enregistre le temps top départ
          Serial.print("Time: ");
          Serial.println(ulngTime);
      }
      else{
          mode = 0;
          Serial.println("The file cannot be created, have to stop recording");
      } 
    }
}

void loop(){
  if (mode == 1){
    if (fileL.available() >= 2){
      if (fileR.available() >= 2){
        byte bufferL[512];
        byte bufferR[512];
        // Fetch 2 blocks from the audio library and copy
        // into a 512 byte buffer.  The Arduino SD library
        // is most efficient when full 512 byte sector size
        // writes are used.
        memcpy(bufferL, fileL.readBuffer(), 256);
        memcpy(bufferR, fileR.readBuffer(), 256);
        fileL.freeBuffer();
        fileR.freeBuffer();
        memcpy(bufferR+256, fileR.readBuffer(), 256);
        memcpy(bufferL+256, fileL.readBuffer(), 256);
        fileL.freeBuffer();
        fileR.freeBuffer();
        frecL.write(bufferR, 512);
        frec.write(bufferL, 512);
      }
    }
    if ((millis() - ulngTime)>= 30000){
      stopRecording();
      initNewFile();
    }
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  fileL.end();
  fileR.end();
  if (mode == 1) {
    while (fileL.available() > 0 && fileR.available() > 0) {
      //frec.write((byte*)fileL.readBuffer(), 256);
      fileL.readBuffer();
      fileL.freeBuffer();
      fileR.readBuffer();
      fileR.freeBuffer();
    }
    frec.close();
    frecL.close();
  }
  mode = 0;
}
