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
TimerClass bootSequenceTimer( 1000 ); //For general output use
uint8_t bootSequenceCounter = 0;
TimerClass sparkleTimer( 10 ); //For general output use
TimerClass washOutTimer( 10 );
TimerClass debugTimer(80); //For general output use
uint16_t tempTimer = 0;

uint16_t msTicks = 0;
uint8_t msTicksMutex = 1; //start locked out

#define MAXINTERVAL 2000 //Max TimerClass interval

//**Color state machines**********************//
#include "colorMixer.h"
#include "colorMachines.h"
int16_t bgOffset = 0;
FlashDialog messages;
float pointBrightness = 0;
int8_t pointPolarity = 1;
BrushPixel sparklePage[8];
uint8_t sparkleCounter = 0;
uint8_t sparkleCounterMax = 100;

WashOut redWash;
WashOut greenWash;
WashOut blueWash;

//**Action Machines***************************//
#include "actionMachines.h"
MainMachine mainSM;
BeatStateMachine beatSM;

//**Accelerometer integration and sensor******//
#include <SparkFunLSM6DS3.h>
#include "accelMaths.h"
#include "Wire.h"
#include "SPI.h"

AccelMaths myIMU( SPI_MODE, 10 );

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
	//myIMU.settings.commInterface = SPI_MODE;
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

	//Make up the washes
	redWash.targetColor.red = 255;
	redWash.targetColor.alpha = 255;
	greenWash.targetColor.green = 255;
	greenWash.targetColor.alpha = 255;
	blueWash.targetColor.blue = 255;
	blueWash.targetColor.alpha = 255;

}

void loop()
{
	// main program

	if ( msTicksMutex == 0 ) //Only touch the timers if clear to do so.
	{
		//**Copy to make a new timer******************//
		//    msTimerA.update(msTicks);
		messageTimer.update(msTicks);
		ledOutputTimer.update(msTicks);
		stateTickTimer.update(msTicks);
		accelReadTimer.update(msTicks);
		bootSequenceTimer.update(msTicks);
		sparkleTimer.update(msTicks);
		washOutTimer.update(msTicks);
		debugTimer.update(msTicks);
		
		//Done?  Lock it back up
		msTicksMutex = 1;
	}  //The ISR should clear the mutex.

	//**Copy to make a new timer******************//
	//  if(msTimerA.flagStatus() == PENDING)
	//  {
	//    digitalWrite( LEDPIN, digitalRead(LEDPIN) ^ 1 );
	//  }

	if (messageTimer.flagStatus() == PENDING)
	{
		messages.tick();
	}

	if (ledOutputTimer.flagStatus() == PENDING)
	{
		//Reset the field
		outputMixer.clearPage();
		//Start with a background if you want
		//outputMixer.addLayer((RGBA8*)background); //Background const array or somethin'

		//Build an image here
		//  Define color registers
		RGBA8 bgColor1;
		bgColor1.green = 255;
		bgColor1.blue = 255;
		bgColor1.red = 255;
		bgColor1.alpha = 10;

		RGBA8 point1;
		RGBA8 point2;
		
		RGBA8 testField[8];
		testField[2].clear();
		testField[2].red = 100;
		testField[2].alpha = 255;
		
		//  Apply operations
		outputMixer.clearPage();
		outputMixer.addLayer(bgColor1);
		//outputMixer.addLayer(testField);

		testField[2].clear();
		testField[2].green = 100;
		testField[2].alpha = 255;
		//outputMixer.addLayer(testField);
		
		point1.green = 0;
		point1.blue = 80;
		point1.red = 0;
		point1.alpha = 120;
		
		point2.green = 0;
		point2.blue = 0;
		point2.red = 80;
		point2.alpha = 120;
		
		if( bootSequenceCounter > 3 )
		{
			outputMixer.gradientAddLayer( point1, 7, point2 , 4 );
			outputMixer.gradientAddLayer( point1, 0, point2 , 3 );
		}
		
		if( bootSequenceCounter > 6 )
		{		
			outputMixer.rotate( bgOffset );
		}
		
		if(( bootSequenceCounter > 9 )&&( bootSequenceCounter < 21 )) //Green blob
		{
			point1.green = 100;
			point1.blue = 0;
			point1.red = 30;
			point1.alpha = (uint8_t)(pointBrightness * 200);
			outputMixer.brush( 2, point1, 2 );
		}
		
		if( bootSequenceCounter > 20 )// Sparkle
		{
			if( bootSequenceCounter < 40 )
			{
				outputMixer.clearPage();
			}
			for( int i = 0; i < 8; i++ )
			{
				outputMixer.brush( i, sparklePage[i], sparklePage[i].sideWall );
			}
		}
		
		outputMixer.addLayer( redWash.outputColor );
		outputMixer.addLayer( greenWash.outputColor );
		outputMixer.addLayer( blueWash.outputColor );

		//Push the output
		outputMixer.mix();
		outputMixer.show();

	}
	if (stateTickTimer.flagStatus() == PENDING)
	{
		mainSM.tick();
		beatSM.tick();
		//Hardcode servicing
		if ( beatSM.exitFlag == 1 )
		{
			beatSM.servicedFlag = 1;
		}

	}
	if (accelReadTimer.flagStatus() == PENDING)
	{
		myIMU.tick();

		mainSM.rightIn = myIMU.rollingAverageRight();
		beatSM.upIn = myIMU.milliDeltaAverageUp();
		beatSM.upDeltaIn = myIMU.milliDeltaDeltaAverageUp();

		if (mainSM.serialOutputEnable == 1)
		{
			Serial.print("\n");
		}
		if (beatSM.risingFlag == 1)
		{
		}
		else
		{

		}
	}
	if (bootSequenceTimer.flagStatus() == PENDING)
	{
		if( bootSequenceCounter < 100 )
		{
			bootSequenceCounter++;
		}
		else
		{
			bootSequenceCounter = 0;
		}
	}
	if (sparkleTimer.flagStatus() == PENDING)
	{
		//Rando table
		// () wait up to 500ms... at 10ms a number 0 to 50
		// ()
		//
		//
		//Process the page
		for( int i = 0; i < 8; i++ )
		{
			//Work each pixel
			sparklePage[i].intensity = sparklePage[i].intensity + sparklePage[i].rate;
			if( sparklePage[i].intensity < 0 )
			{
				sparklePage[i].intensity = 0;
			}
			if( sparklePage[i].intensity > sparklePage[i].maxIntensity )
			{
				sparklePage[i].intensity = sparklePage[i].maxIntensity;
				sparklePage[i].rate = sparklePage[i].rate * -1;
			}
			sparklePage[i].alpha = sparklePage[i].intensity * 150;
		}
		if( sparkleCounter < sparkleCounterMax )
		{
			sparkleCounter++;
		}
		else
		{
			//set a new delay
			sparkleCounter = 0;
			sparkleCounterMax = random(25);
			
			//generate a new brush
			uint8_t tempLocation = random(8);
			sparklePage[tempLocation].red = 153;
			sparklePage[tempLocation].green = 48;
			sparklePage[tempLocation].blue = 191;
			sparklePage[tempLocation].sideWall = random(5);
			sparklePage[tempLocation].maxIntensity = .3 + .7 * (float)random(100)/100;
			float tempRate = 0.12 - 0.06 * ((float)random(100)/100);
			//if( sparklePage[tempLocation].rate < tempRate )
			//{
			//	if( sparklePage[tempLocation].intensity == 0 )
			//	{
					sparklePage[tempLocation].rate = tempRate;
			//	}
			//}
			
		}
	
	}
	if (washOutTimer.flagStatus() == PENDING )
	{
		//Service all the washouts
		redWash.tick();
		greenWash.tick();
		blueWash.tick();
		tempTimer++;
		if( tempTimer == 1000 )
		{
			greenWash.trigger();
		}
		if( tempTimer == 1200 )
		{
			redWash.trigger();
		}
		if( tempTimer == 1300 )
		{
			blueWash.trigger();
		}
		if( tempTimer > 1355 )
		{
			tempTimer = 0;
		}

	}
	if (debugTimer.flagStatus() == PENDING)
	{
		bgOffset = bgOffset + 1;
		if( bgOffset > 7 )
		{
			bgOffset = 0;
		}
		pointBrightness = pointBrightness + ( pointPolarity *  0.025 );
		if( pointBrightness > 1 )
		{
			pointBrightness = 1;
			pointPolarity = -1;
		}
		if( pointBrightness < 0 )
		{
			pointBrightness = 0;
			pointPolarity = 1;
		}

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
	if (msTicks >= ( MAXTIMER + MAXINTERVAL ))
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


