/*
  Biomedical Engineering BME468 - Final Project
  Michael Twardowski and John Pfisterer


  Smoothing
  Reads repeatedly from an analog input, calculating a running average
  and printing it to the computer.  Keeps ten readings in an array and
  continually averages them.

  Igoe
  http://www.arduino.cc/en/Tutorial/Smoothing
*/

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.

#include <Servo.h>

Servo servothumb,
      servoindex,
      servomajeure,
      servoringfinger,
      servopinky,
      servowrist;

//const int numReadings = 30;
const int sampleWindow = 150; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

//int readings[numReadings];      // the readings from the analog input
//int index = 0;                  // the index of the current reading
//int total = 0;                  // the running total
//int average = 0;                // the average
boolean oopen = false;
//int inputPin = A0;


void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  servothumb.attach(6); // Set left servo to digital pin 10
  servoindex.attach(5); // Set right servo to digital pin 9
  servomajeure.attach(4);
  servoringfinger.attach(3);
  servopinky.attach(2);
  servowrist.attach(7);
}

void loop() {

  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int servo = 0;
  unsigned int val = 0;
  unsigned int val2 = 0;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(1);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }

  Serial.print(" MAX: ");
  Serial.print(signalMax);
  Serial.print(" MIN: ");
  Serial.print(signalMin);
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  Serial.print(" peak-peak:  ");
  Serial.print(peakToPeak);
  //  double volts = (peakToPeak * 3.3) / 1024;  // convert to volts

  val = map(peakToPeak, 0, 1023, 1024, 0 );    // invert range
  Serial.print(" val:  ");
  Serial.print(val);
  val2 = map(val, 0, 1023, 0, 180 );            // maps current range to what we want
  //  val = map(val, signalMin, signalMax, 0, 180 );
  //   val = val/ 5.5;
  Serial.print(" val2:  ");
  Serial.println(val2);
  if (val2 > 180)
  {
    val2 = 180;
  }
  servothumb.write(val2);
  servoindex.write(val2);
  servomajeure.write(val2);
  servoringfinger.write(val2);
  servopinky.write(val2);
}

