/*
  Biomedical Engineering BME468 - Final Project
  Michael Twardowski and John Pfisterer

  EMG Control
*/

#include <Servo.h>

Servo firstPhalange,
      secondPhalange,
      thirdPhalange,
      fourthPhalange,
      fifthPhalange,
      wrist;

/**
    Holds sample data from each channel and saves maximum and minimum values
*/
unsigned int handFlexMax = 0,
             handFlexMin = 1024,
             handFlexPeak = 0,
             handFlexMap = 0,
             handExtendMax = 0,
             handExtendMin = 1024,
             handExtendPeak = 0,
             handExtendMap = 0,
             wristMax = 0,
             wristMin = 1024,
             wristPeak = 0,
             wristMap = 0;

/**
 * Arrays for holding data
 */
unsigned int handFlex[5],
             handExtend[5],
             wristRotation[5];

/**
 * data index
 */
unsigned int dataIndex = 0;
               

/**
 * Sample Period in mS
 */
const unsigned int SAMPLE_PERIOD = 50;

/**
 * Servo update interval
 */
const unsigned int SERVO_INTERVAL = 200;

/**
 * Keeps track of the last time each task was completed.
 */
 unsigned long lastSampleTime = 0,
               lastServoUpdateTime = 0;

/**
 * Hand State
 */
 boolean isHandOpen = false;
              
void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  firstPhalange.attach(6);
  secondPhalange.attach(5);
  thirdPhalange.attach(4);
  fourthPhalange.attach(3);
  fifthPhalange.attach(2);
  wrist.attach(7);
  openHand();
}

void loop() {

  if(cycleCheck(&lastSampleTime, SAMPLE_PERIOD)){
    sampleData();
  }

  /*
  checkAmplitude(handFlex, handFlexMax, handFlexMin);
  checkAmplitude(handExtend, handExtendMax, handExtendMin);
  checkAmplitude(wristRotation, wristMax, wristMin);

  handFlexPeak = peakToPeak(handFlexMax, handFlexMin);
  handExtendPeak = peakToPeak( handExtendMax, handExtendMin);
  wristPeak = peakToPeak(wristMax, wristMin);

  handFlexMap = mapValues(handFlexPeak);
  handExtendMap = mapValues(handExtendPeak);
  wristMap = mapValues(wristPeak);
  */
  
  if(cycleCheck(&lastServoUpdateTime, SERVO_INTERVAL)){
     unsigned int handFlexMedian = medianFilter(handFlex);
                  //handExtendMedian = medianFilter(handExtend),
                 // wristRotationMedian = medianFilter(wristRotation);
    
     handFlexMap = mapValues(handFlexMedian);
     //handExtendMap = mapValues(handExtendMedian);
     //wristMap = mapValues(wristRotationMedian);
     if(isHandOpen && handFlexMedian > 200){
        closeHand();
     } else if(!isHandOpen & handFlexMedian < 50){
        openHand();
     }
  }
  
}

boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle) 
{
 unsigned long currentMillis = millis();
 if(currentMillis - *lastMillis >= cycle)
 {
   *lastMillis = currentMillis;
   return true;
 }
 else
   return false;
}


/**
 * Samples the data from each EMG channel
 */
void sampleData(){
    handFlex[dataIndex] = analogRead(1);
    //handExtend[dataIndex] = analogRead(2);
    //wristRotation[dataIndex] = analogRead(0);
    /*
    Serial.println(dataIndex);
    Serial.print("Analog Read: ");
    Serial.println(handFlex[dataIndex]);
    */
    dataIndex++;
    if(dataIndex > 4){
      dataIndex = 0;
    }
    /*Serial.print("Data: ");
    for(int i = 0; i < 5; i++){
      Serial.print(handFlex[i]);
      Serial.print(" ");
    }
    Serial.println("----");
    */
}

/**
 * Updates the servo position
 */
void closeHand(){
  Serial.println();
  Serial.println("Close Hand");
  Serial.println();
  firstPhalange.write(0);
  secondPhalange.write(0);
  thirdPhalange.write(0);
  fourthPhalange.write(0);
  fifthPhalange.write(0);

  isHandOpen = false;
}

/**
 * Updates the servo position
 */
void openHand(){
  Serial.println();
  Serial.println("Open Hand");
  Serial.println();
  firstPhalange.write(180);
  secondPhalange.write(180);
  thirdPhalange.write(180);
  fourthPhalange.write(180);
  fifthPhalange.write(180);

  isHandOpen = true;
}


/**
 * Median filters data
 */
 unsigned int medianFilter(unsigned int data[]){

 int dataTemp[5];
 for(int k=0; k<5; k++){
    dataTemp[k]=data[k];
}
// selection sort that stops after the middle index is sorted
// expects an input array of 5  
  int i, j, maxIndex, tmp;    
    for (i = 0; i < 3; i++) {
          maxIndex = 0;
          for (j = 0; j < 5 - i; j++){
                if (dataTemp[j] > dataTemp[maxIndex]){
                      maxIndex = j;
                }
          }
          tmp = dataTemp[4 - i];
          dataTemp[4 - i] = dataTemp[maxIndex];
          dataTemp[maxIndex] = tmp;
    }
  //Serial.println("----");
  Serial.print("Median Filter: ");
  Serial.println(dataTemp[2]);
   /*for(int i = 0; i < 5; i++){
      Serial.print(dataTemp[i]);
      Serial.print(" ");
    }
    Serial.println("");
    Serial.println("");
    */
  return dataTemp[2];
 }

/**
   Checks amplitude of input signal and records maximum and minimum values
*/
void checkAmplitude(unsigned int value, unsigned int &maxValue, unsigned int &minValue) {
  if (value < 1024)  // toss out spurious readings
  {
    if (value > maxValue)
    {
      maxValue = value;  // save just the max levels
    }
    else if (value < minValue)
    {
      minValue = value;  // save just the min levels
    }
  }
}

/**
   Finds the Peak to Peak voltage
*/
unsigned int peakToPeak(unsigned int maxValue, unsigned int minValue) {
  unsigned int range = maxValue - minValue;  // max - min = peak-peak amplitude
  return range;
}

/**
   Checks amplitude of input signal and records maximum and minimum values
*/
unsigned int mapValues(unsigned int range) {
  unsigned int newMap = map(range, 0, 1023, 1024, 0 );    // invert range
  unsigned int theMap = map(newMap, 0, 1023, 0, 180 );            // maps current range to what we want
  if (theMap > 180) {
    theMap = 180;
  }else if (theMap < 0){
    theMap = 0;
  }
  return theMap;
}
