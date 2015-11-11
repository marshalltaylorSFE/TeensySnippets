#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputAnalog         adc1;           //xy=117,436
AudioMixer4              mixer2;         //xy=359,456
AudioEffectBitcrusher    bitcrusher1;    //xy=421,287
AudioAnalyzePeak         peak1;          //xy=536,540
AudioFilterStateVariable filter2;        //xy=586,354
AudioEffectDelay         delay1;         //xy=715,612
AudioMixer4              mixer1;         //xy=719,443
AudioFilterStateVariable filter1;        //xy=883,480
AudioOutputAnalog        dac1;           //xy=1058,494
AudioConnection          patchCord1(adc1, 0, mixer2, 0);
AudioConnection          patchCord2(mixer2, bitcrusher1);
AudioConnection          patchCord3(mixer2, peak1);
AudioConnection          patchCord4(mixer2, 0, mixer1, 1);
AudioConnection          patchCord5(bitcrusher1, 0, filter2, 0);
AudioConnection          patchCord6(filter2, 0, mixer1, 0);
AudioConnection          patchCord7(delay1, 0, mixer1, 2);
AudioConnection          patchCord8(delay1, 1, mixer1, 3);
AudioConnection          patchCord9(mixer1, delay1);
AudioConnection          patchCord10(mixer1, 0, filter1, 0);
AudioConnection          patchCord11(filter1, 0, dac1, 0);
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

  filter2.octaveControl(7);
  filter2.frequency(4000);

  filter1.octaveControl(7);
  filter1.frequency(15000);

  bitcrusher1.bits(6);
  bitcrusher1.sampleRate(44100);
  
  delay1.delay(0, 50);
  delay1.delay(1, 40);
  
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.01);
  mixer1.gain(3, 0.01);

  mixer2.gain(0, 1);
  
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
		temp = temp / 0.3;
		if( temp > 1 )
		{
			temp = 1;
		}
		ledPW = temp * 100;
		
		float lowertemp;
		float uppertemp;
		lowertemp = ( 1 + knob0Value * 15 );
		uppertemp = ( knob1Value * temp * 15 );
		//reassign temp
		temp = lowertemp + uppertemp;
		if( temp > 16 )
		{
			temp = 16;
		}
		bitcrusher1.bits( temp );
		//bitcrusher1.sampleRate(44100 - 44100 * temp );
		//Serial.println(temp);
    }

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



