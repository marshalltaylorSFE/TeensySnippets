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

//**General***********************************//
#include "stdint.h"

//**Timers and stuff**************************//
#include "timerModule.h"

IntervalTimer myTimer;

TimerClass messageTimer( 10 );
TimerClass ledOutputTimer( 5 ); //For updating LEDs
TimerClass stateTickTimer( 2 ); //Do color state maths
TimerClass accelReadTimer( 2 ); //read the sensor and integrate
TimerClass debugTimer(1000);

uint16_t msTicks = 0;
uint8_t msTicksMutex = 1; //start locked out

#define MAXINTERVAL 2000 //Max TimerClass interval

//**Color state machines**********************//
#include "colorMixer.h"
#include "colorMachines.h"

FlashDialog messages;

//**Action Machines***************************//
#include "actionMachines.h"
MainMachine mainSM;
BeatStateMachine beatSM;

//**Color pages
const RGBA8_t background[8] = {30, 5, 0, 255,    20, 12, 10, 255,    10, 12, 20, 255,    0, 5, 30, 255,    0, 5, 30, 255,    10, 12, 20, 255,    20, 12, 10, 255,    30, 5, 0, 255};
RGBA8_t testColor;
RGBA8_t messageColor;

//**Accelerometer integration and sensor******//
#include <SparkFunLSM6DS3.h>
#include "accelMaths.h"
#include "Wire.h"
#include "SPI.h"

AccelMaths myIMU;

//**Color output stuff************************//
#include <Adafruit_NeoPixel.h>
#include "colorMixer.h"
#define WS2812PIN 3
ColorMixer outputMixer = ColorMixer(60, WS2812PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  delay(2000);

  Serial.begin(115200);
  Serial.println("Program started\n");

  // initialize IntervalTimer interrupt stuff
  myTimer.begin(serviceMS, 1000);  // serviceMS to run every 0.001 seconds

  //Select bus if nothing else.
  myIMU.settings.commInterface = SPI_MODE;
  //Call .begin() to configure the IMU
  myIMU.settings.accelEnabled = 1;
  myIMU.settings.accelODROff = 0;  //Set to disable ODR (control sample rate)
  myIMU.settings.accelRange = 8;      //Max G force readable.  Can be: 2, 4, 8, 16
  myIMU.settings.accelSampleRate = 104;  //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
  myIMU.settings.accelBandWidth = 200;  //Hz.  Can be: 50, 100, 200, 400;
  myIMU.settings.accelFifoEnabled = 1;  //Set to include accelerometer in the FIFO
  myIMU.settings.accelFifoDecimation = 1;  //set 1 for on /1
  myIMU.settings.tempEnabled = 1;
  myIMU.begin();
  
  outputMixer.begin();
  outputMixer.show();
  
  //messageColor.red = 128;
  
  
}

void loop()
{
  // main program
  
  if( msTicksMutex == 0 )  //Only touch the timers if clear to do so.
  {
//**Copy to make a new timer******************//  
//    msTimerA.update(msTicks);
	messageTimer.update(msTicks);
    ledOutputTimer.update(msTicks);
	stateTickTimer.update(msTicks);
	accelReadTimer.update(msTicks);
	debugTimer.update(msTicks);

 //Done?  Lock it back up
    msTicksMutex = 1;
  }  //The ISR should clear the mutex.
  
//**Copy to make a new timer******************//  
//  if(msTimerA.flagStatus() == PENDING)
//  {
//    digitalWrite( LEDPIN, digitalRead(LEDPIN) ^ 1 );
//  }
  
  if(messageTimer.flagStatus() == PENDING)
  {
    messages.tick();
  }
  
  if(ledOutputTimer.flagStatus() == PENDING)
  {
    //Push the output
    outputMixer.mix();
    outputMixer.show();
    //Reset the field
    outputMixer.clearPage();
    outputMixer.addLayer((RGBA8_t*)background);
	  //go get newest color
	if(mainSM.serialOutputEnable == 0 )
	{
	  testColor.blue = 0;
	  messages.enabled = 0;
	  if(beatSM.risingFlag == 1)
	  {
	    testColor.green = 255;
	  }
	  else
	  {
	    testColor.green = 0;
	  }
	  
	}
	else
	{
	  testColor.green = 0;
	  testColor.blue = 64;
	  messages.enabled = 1;
	}
    outputMixer.orLayer(testColor);
	  //apply the messages
	messageColor.red = messages.red >> 1;
	outputMixer.orLayer(messageColor);
	
  }
  if(stateTickTimer.flagStatus() == PENDING)
  {
	mainSM.tick();
	beatSM.tick();
	//Hardcode servicing
	if( beatSM.exitFlag == 1 )
	{
		beatSM.servicedFlag = 1;
	}
	
  }
  if(accelReadTimer.flagStatus() == PENDING)
  {
    myIMU.tick();

	mainSM.rightIn = myIMU.rollingAverageRight();
	beatSM.upIn = myIMU.milliDeltaAverageUp();
	beatSM.upDeltaIn = myIMU.milliDeltaDeltaAverageUp();
	
	if(mainSM.serialOutputEnable == 1)
	{
	    Serial.print("\n");
		//Serial.print(myIMU.readFloatAccelY(), 4);
		//Serial.print(",");
	    Serial.print(myIMU.rollingAverageUp(), 4);
		Serial.print(",");
	    Serial.print(myIMU.milliDeltaAverageUp(), 4);
		//Serial.print(",");
	    //Serial.print(myIMU.milliDeltaDeltaAverageUp(), 4);
	}
	if(beatSM.risingFlag == 1)
	{
	}
	else
	{
	  
	}
  }
  if(debugTimer.flagStatus() == PENDING)
  {
    //Get all parameters
//  Serial.print("\nPos:\n");
//  Serial.print(" X = ");
//  Serial.println(myIMU.scaledXX(), 4);
//
//  Serial.print("\nThermometer:\n");
//  Serial.print(" Degrees C = ");
//  Serial.println(myIMU.readTempC(), 4);
//  Serial.print(" Degrees F = ");
//  Serial.println(myIMU.readTempF(), 4);

	//Serial.println(mainSM.rightIn, 4);
	//Serial.println(testColor.red);

  }  
  
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
  
}


