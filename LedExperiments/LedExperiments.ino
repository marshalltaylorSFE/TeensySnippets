//**********************************************************************//
//  BEERWARE LICENSE
//
//  This code is free for any use provided that if you meet the author
//  in person, you buy them a beer.
//
//  This license block is BeerWare itself.
//
//  Written by:  Marshall Taylor
//  Created:  March 21, 2015
//
//**********************************************************************//

//HOW TO OPERATE
//  Make TimerClass objects for each thing that needs periodic service
//  pass the interval of the period in ticks
//  Set MAXINTERVAL to the max foreseen interval of any TimerClass
//  Set MAXTIMER to overflow number in the header.  MAXTIMER + MAXINTERVAL
//    cannot exceed variable size.

#define LEDPIN 13
#include "timerModule.h"
#include "stdint.h"
#include "ledTuning.h"
#include "colorTools.h"
#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"

#define REDPIN 9
#define GREENPIN 16
#define BLUEPIN 17

LedTable myLedTable( 5 );
ColorPoint myPoint;
//Globals
IntervalTimer myTimer;
LSM6DS3 myIMU;

TimerClass msTimerA( 20 );
TimerClass msTimerB( 21 );

uint16_t msTicks = 0;
uint8_t msTicksMutex = 1; //start locked out

#define MAXINTERVAL 2000

void setup()
{
  //Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);

  // initialize IntervalTimer
  myTimer.begin(serviceMS, 1000);  // serviceMS to run every 0.001 seconds

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  digitalWrite(REDPIN, 0);
  digitalWrite(GREENPIN, 0);
  digitalWrite(BLUEPIN, 0);

  myIMU.settings.gyroEnabled = 0;  //Can be 0 or 1

  myIMU.settings.accelEnabled = 1;
  myIMU.settings.accelRange = 8;      //Max G force readable.  Can be: 2, 4, 8, 16
  myIMU.settings.accelSampleRate = 1666;  //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
  myIMU.settings.accelBandWidth = 50;  //Hz.  Can be: 50, 100, 200, 400;

  myIMU.settings.commInterface = SPI_MODE;

  delay(2000);

  myIMU.begin();

  myLedTable.calcTable();
  
  
  
}

int i = 0;
int intervalSeed = 20;
void loop()
{
  // main program
  
  if( msTicksMutex == 0 )  //Only touch the timers if clear to do so.
  {
    msTimerA.update(msTicks);
    msTimerB.update(msTicks);
    //Done?  Lock it back up
    msTicksMutex = 1;
  }  //The ISR should clear the mutex.
  
  
  if(msTimerA.flagStatus() == PENDING)
  {
    digitalWrite( LEDPIN, digitalRead(LEDPIN) ^ 1 );
  }
  if(msTimerB.flagStatus() == PENDING)
  {
    digitalWrite( LEDPIN, digitalRead(LEDPIN) ^ 1 );
  }
  i++;
  delay(1);
  if(i > 100)
  {
    i = 0;
    msTimerA.setInterval(intervalSeed);
    intervalSeed++;
    msTimerB.setInterval(intervalSeed);

  }


  float zValue = 0;

  //delay(10);

  zValue = myIMU.readFloatAccelY();
  
  if ( zValue < -8 ) zValue = -8;
  if ( zValue > 8 ) zValue = 8;
  
  myPoint.redF = 3200 * zValue;

 
}

void serviceMS(void)
{
  uint32_t returnVar = 0;
  if(msTicks >= ( MAXTIMER + MAXINTERVAL ))
  {
    returnVar = msTicks - MAXTIMER;

  }
  else
  {
    returnVar = msTicks + 1;
  }
  msTicks = returnVar;
  msTicksMutex = 0;  //unlock
  
  myPoint.tick();
  analogWrite(REDPIN, myLedTable.ledAdjust(myPoint.redP + 128)); 
  analogWrite(BLUEPIN, myLedTable.ledAdjust(255 - myPoint.redP + 128)); 

}


