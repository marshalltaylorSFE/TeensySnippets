#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputAnalog         adc1;           //xy=117,436
AudioFilterBiquad        biquad1;        //xy=324,385
AudioFilterBiquad        biquad2;        //xy=325,479
AudioAnalyzePeak         peak1;          //xy=355,585
AudioMixer4              mixer1;         //xy=501,54
AudioEffectMultiply      multiply1;      //xy=537,433
AudioOutputAnalog        dac1;           //xy=720,143
AudioConnection          patchCord1(adc1, biquad1);
AudioConnection          patchCord2(adc1, biquad2);
AudioConnection          patchCord3(adc1, peak1);
AudioConnection          patchCord4(adc1, 0, mixer1, 0);
AudioConnection          patchCord5(biquad1, 0, multiply1, 0);
AudioConnection          patchCord6(biquad2, 0, multiply1, 1);
AudioConnection          patchCord7(mixer1, dac1);
AudioConnection          patchCord8(multiply1, 0, mixer1, 1);
// GUItool: end automatically generated code



//Not used by this sketch but dependant on one 
#include "Wire.h"

//Globals
uint32_t MAXTIMER = 60000000;
uint32_t MAXINTERVAL = 2000000;

#define LEDPIN 13
#define knob0Pin A15
#define knob1Pin A16
float knob0Value = 0;
float knob1Value = 0;

#include "timerModule32.h"
#include "stdint.h"

IntervalTimer myTimer; //Interrupt for Teensy

uint8_t ledPW = 50;
uint8_t ledPhase = 0;

//**Copy to make a new timer******************//  
//TimerClass32 usTimerA( 20000 ); //20 ms
TimerClass32 usTimerA( 10 ); //330 ms
TimerClass32 usTimerB( 5000 );
TimerClass32 usTimerC( 1000000 );
//Note on TimerClass-
//Change with usTimerA.setInterval( <the new interval> );
uint32_t filterfreq = 1;
float filterq = 0.8;
uint32_t usTicks = 0;

//  The lock works like this:
//
//    When the interrupt fires, the lock is removed.  Now
//    the main free-wheeling loop can update the change to
//    the timerModules.  Once complete, the lock is replaced
//    so that it can't update more than once per firing
//    of the interrupt

uint8_t usTicksLocked = 1; //start locked out
void setup()
{
  Serial.begin(57600);
  pinMode(LEDPIN, OUTPUT);
  pinMode(knob0Pin, INPUT);
  pinMode(knob1Pin, INPUT);
  
  // initialize IntervalTimer
  myTimer.begin(serviceUS, 1);  // serviceMS to run every 0.000001 seconds

  // Audio connections require memory to work. For more  
  // detailed information, see the MemoryAndCpuUsage example  
  AudioMemory(100);
  
  biquad1.setBandpass(0, 500, 0.7);
  biquad1.setBandpass(1, 500, 0.7);
  biquad1.setBandpass(2, 500, 0.7);
  biquad1.setBandpass(3, 500, 0.7);
  biquad2.setLowpass(0, 240, 0.7);
  biquad2.setLowpass(1, 240, 0.7);
  biquad2.setLowpass(2, 240, 0.7);
  biquad2.setLowpass(3, 240, 0.7);
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  
  
}

void loop()
{
  //Update the timers, but only once per interrupt
  if( usTicksLocked == 0 )
  {
    //**Copy to make a new timer******************//  
    //msTimerA.update(usTicks);
    
    usTimerA.update(usTicks);
    usTimerB.update(usTicks);
    usTimerC.update(usTicks);
    //Done?  Lock it back up
    usTicksLocked = 1;
  }  //The ISR will unlock.

  //**Copy to make a new timer******************//  
  //if(usTimerA.flagStatus() == PENDING)
  //{
  //  //User code
  //}
  
  if(usTimerA.flagStatus() == PENDING)
  {
    //User code
	ledPhase++;
	if(ledPhase > 100)
	{
		ledPhase = 0;
	}
	if(ledPhase > ledPW)
	{
		digitalWrite( LEDPIN, 0 );
	}
	else
	{
		digitalWrite( LEDPIN, 1 );
	}
  }
  if(usTimerB.flagStatus() == PENDING)
  {
    //User code
	knob0Value = 1 - (float)analogRead(knob0Pin) / 60000;
	knob1Value = 1 - (float)analogRead(knob1Pin) / 60000;
	//mixer2.gain(0, knob1Value * 1);
	if (peak1.available())
	{
		float temp = peak1.read();
		temp = temp;
		if( temp > 1 )
		{
			temp = 1;
		}
		ledPW = temp * 100;
		
    }
	mixer1.gain(0, knob0Value * 0.5 );
	mixer1.gain(1, 0.5 - (knob0Value * 0.5) );

	

  }
  if(usTimerC.flagStatus() == PENDING)
  {
    //User code
	Serial.print(knob0Value);
	Serial.print(",");
	Serial.print(knob1Value);
	Serial.print("\n");
	
  }


}

void serviceUS(void)
{
  uint32_t returnVar = 0;
  if(usTicks >= ( MAXTIMER + MAXINTERVAL ))
  {
    returnVar = usTicks - MAXTIMER;

  }
  else
  {
    returnVar = usTicks + 1;
  }
  usTicks = returnVar;
  usTicksLocked = 0;  //unlock
}



