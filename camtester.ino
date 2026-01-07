#include <SD.h>
#include <SPI.h>

#define MIC_PIN A1      // Microphone input
#define SD_CS 10
const int DAC_PIN = A0;  // DAC output
const int next = 3;      //next button
const int replay = 2;    //replay button
const int BUFFER_SIZE = 512; //size of buffer for reading/writing audio data
byte buffer[BUFFER_SIZE];

File wavFile;
File audioFile;

// WAV settings
const unsigned long SAMPLE_RATE = 8500;  //sample rate in hz 
const unsigned long SAMPLE_PERIOD_US = 1000000UL / SAMPLE_RATE; //time between samples

//index for randomly chosen question for all questions
int q1_index;
int q2_index; 
int q3_index;
int q4_index;

//array of filenames for different variations of questions
const char* question1_names[] = {
  "Q1-1.raw",
  "Q1-2.raw",
  "Q1-3.raw",
  "Q1-4.raw",
};

const char* question2_names[] = {
  "Q2-1.raw",
  "Q2-2.raw",
  "Q2-3.raw",
  "Q2-4.raw",
};

const char* question3_names[] = {
  "Q3-1.raw",
  "Q3-2.raw",
};

const char* question4_names[] = {
  "Q4-1.raw",
  "Q4-2.raw",
  "Q4-3.raw",
  "Q4-4.raw",
};

//array of filenames for different variations of answers
const char* answer1_names[] = {
  "A1-1.raw",
  "A1-2.raw",
  "A1-3.raw",
  "A1-4.raw",
};

const char* answer2_names[] = {
  "A2-1.raw",
  "A2-2.raw",
  "A2-3.raw",
};

const char* answer3_names[] = {
  "A3-1.raw",
  "A3-2.raw",
};

const char* answer4_names[] = {
  "A4-1.raw",
  "A4-2.raw",
  "A4-3.raw",
  "A4-4.raw",
};

void setup() {
  Serial.begin(115200);
  SD.begin(SD_CS);

  Serial.println("SD OK");
  pinMode(next, INPUT_PULLUP);
  pinMode(replay, INPUT_PULLUP);
  pinMode(DAC_PIN, OUTPUT);
  
  //randomly choose questions from question bank
  q1_index = random(4);
  q2_index = random(3);
  q3_index = random(2);
  q4_index = random(4);
}

void loop() {

  //play system introduction audio
  wavFile = SD.open("intro1.raw"); 
  playQuestion(wavFile);

  //wait for patient to click next button
  while(1){
    if (digitalRead(next) == LOW){
      break;
    }
  }

  //play the randomly selected question 1
  wavFile = SD.open(question1_names[q1_index]); 
  playQuestion(wavFile);

  //wait for next button or replay button to be selected
  while(1){
    if (digitalRead(next) == LOW){
      break;
    }

    if (digitalRead(replay) == LOW){
      wavFile = SD.open(question1_names[q1_index]); 
      playQuestion(wavFile);
    }
  }
  //record patient's answer to q1
  audioFile = SD.open(answer1_names[q1_index], FILE_WRITE);
  recordAnswer(audioFile);
  audioFile.close();


  //play the randomly selected question 1
  wavFile = SD.open(question2_names[q2_index]); 
  playQuestion(wavFile);
  
  //wait for next button or replay button to be selected
  while(1){
    if (digitalRead(next) == LOW){
      break;
    }

    if (digitalRead(replay) == LOW){
      wavFile = SD.open(question2_names[q2_index]); 
      playQuestion(wavFile);
    }
  }
  
  //record patient's answer to q2
  audioFile = SD.open(answer2_names[q2_index], FILE_WRITE);
  recordAnswer(audioFile);

  //play the randomly selected question 3
  wavFile = SD.open(question3_names[q3_index]); 
  playQuestion(wavFile);

  //wait for next button or replay button to be selected
  while(1){
    if (digitalRead(next) == LOW){
      break;
    }

    if (digitalRead(replay) == LOW){
      wavFile = SD.open(question3_names[q3_index]); 
      playQuestion(wavFile);
    }
  }
  //record patient's answer to q3
  audioFile = SD.open(answer3_names[q3_index], FILE_WRITE);
  recordAnswer(audioFile);

  //play the randomly selected question 4
  wavFile = SD.open(question4_names[q4_index]); 
  playQuestion(wavFile);

  //wait for next button or replay button to be selected
  while(1){
    if (digitalRead(next) == LOW){
      break;
    }

    if (digitalRead(replay) == LOW){
      wavFile = SD.open(question4_names[q4_index]); 
      playQuestion(wavFile);
    }
  }
  //record patient's answer to q4
  audioFile = SD.open(answer4_names[q4_index], FILE_WRITE);
  recordAnswer(audioFile);  

  //play ending audio
  wavFile = SD.open("Ending.raw"); 
  playQuestion(wavFile);

  while(1){

  }
} 

//plays an 8bit mono raw audio file through DAC pin
void playQuestion(File wavFile){
  //loops while there is data avaiable in audio file
  while(wavFile.available()){
    //records the starting time
    unsigned long t0 = micros();

    //read one 8-bit PCM audio sample
    uint8_t sample = wavFile.read();

    //boost sample amplitude
    uint8_t boosted = sample*1.2;
    if (boosted >255){
      boosted = 255;
    }

    //write sample to the DAC pin
    analogWrite(DAC_PIN, boosted);  // write to DAC



    //wait for required sample period to pass to keep sample rate
    while (micros() - t0 < SAMPLE_PERIOD_US)
      ;
  }
  //close the file
  wavFile.close();
}

//records audio from microphone to the SD card until the left button is pressed
void recordAnswer(File audioFile){
  int index = 0;
  unsigned long startTime = millis();

  // Skip button reading check for the first 50ms to prevent immediate stop after button is pressed
  if (millis() - startTime < 50) { 
        
  }

  //loop while left button is not pressed
  while (digitalRead(replay)==HIGH) {
    int sample = analogRead(MIC_PIN);  // 0-1023
    
    //convert sample into 8bit
    byte sample8 = sample >> 2; 

    //store sample in buffer array
    buffer[index++] = sample8;

    //if buffer is full, write entire buffer to sd card
    if (index >= BUFFER_SIZE) {
      audioFile.write(buffer, BUFFER_SIZE);
      index = 0;
    }
  }

  // Write any remaining bytes
  if (index > 0) {
    audioFile.write(buffer, index);
  }

  //close file to save data
  audioFile.close();

}
