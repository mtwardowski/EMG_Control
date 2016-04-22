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
unsigned int handFlex,
             handFlexMax = 0,
             handFlexMin = 1024,
             handFlexPeak = 0,
             handFlexMap = 0,
             handExtend,
             handExtendMax = 0,
             handExtendMin = 1024,
             handExtendPeak = 0,
             handExtendMap = 0,
             wristRotation,
             wristMax = 0,
             wristMin = 1024,
             wristPeak = 0,
             wristMap = 0;

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
              
void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  firstPhalange.attach(6);
  secondPhalange.attach(5);
  thirdPhalange.attach(4);
  fourthPhalange.attach(3);
  fifthPhalange.attach(2);
  wrist.attach(7);
}

void loop() {

  handFlexMax = 0;
  handFlexMin = 1024;
  handFlexPeak = 0;
  handFlexMap = 0;
  
  handExtendMax = 0;
  handExtendMin = 1024;
  handExtendPeak = 0;
  handExtendMap = 0;
  wristRotation = 0;
  
  wristMax = 0;
  wristMin = 1024;
  wristPeak = 0;
  wristMap = 0;

  if(cycleCheck(&lastSampleTime, SAMPLE_PERIOD)){
    sampleData();
  }
  
  checkAmplitude(handFlex, handFlexMax, handFlexMin);
  checkAmplitude(handExtend, handExtendMax, handExtendMin);
  checkAmplitude(wristRotation, wristMax, wristMin);

  handFlexPeak = peakToPeak(handFlexMax, handFlexMin);
  handExtendPeak = peakToPeak( handExtendMax, handExtendMin);
  wristPeak = peakToPeak(wristMax, wristMin);

  handFlexMap = mapValues(handFlexPeak);
  handExtendMap = mapValues(handExtendPeak);
  wristMap = mapValues(wristPeak);
  
  if(cycleCheck(&lastServoUpdateTime, SERVO_INTERVAL)){
    moveServos();
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
  handFlex = analogRead(1);
  handExtend = analogRead(2);
  wristRotation = analogRead(0);
}

/**
 * Updates the servo position
 */
void moveServos(){
  firstPhalange.write(handFlexMap);
  secondPhalange.write(handFlexMap);
  thirdPhalange.write(handFlexMap);
  fourthPhalange.write(handFlexMap);
  fifthPhalange.write(handFlexMap);
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
