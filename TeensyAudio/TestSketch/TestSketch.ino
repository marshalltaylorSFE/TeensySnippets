#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>


// GUItool: begin automatically generated code
AudioSynthWaveform       waveform4;      //xy=275,593
AudioInputAnalog         adc1;           //xy=304,663
AudioSynthWaveform       waveform2;      //xy=343,502
AudioSynthWaveform       waveform1;      //xy=344,461
AudioSynthWaveform       waveform3;      //xy=347,546
AudioMixer4              mixer1;         //xy=623,522
AudioFilterStateVariable filter1;        //xy=821,568
AudioOutputAnalog        dac1;           //xy=1037,562
AudioConnection          patchCord1(adc1, 0, mixer1, 3);
AudioConnection          patchCord2(mixer1, 0, filter1, 0);
AudioConnection          patchCord3(filter1, 0, dac1, 0);
// GUItool: end automatically generated code


// GUItool: end automatically generated code

// GUItool: end automatically generated code

// GUItool: end automatically generated code

// GUItool: end automatically generated code

// GUItool: end automatically generated code

// GUItool: end automatically generated code
//Not used by this sketch but dependant on one 
#include "Wire.h"

//Globals
uint32_t MAXTIMER = 60000000;
uint32_t MAXINTERVAL = 2000000;

#define LEDPIN 13
#include "timerModule32.h"
#include "stdint.h"

IntervalTimer myTimer; //Interrupt for Teensy


//**Copy to make a new timer******************//  
//TimerClass32 usTimerA( 20000 ); //20 ms
TimerClass32 usTimerA( 330000 ); //330 ms
TimerClass32 usTimerB( 1000 );
TimerClass32 usTimerC( 3000 );
//Note on TimerClass-
//Change with usTimerA.setInterval( <the new interval> );
uint32_t filterfreq = 1;
float filterq = 0.7;
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
  //Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  // initialize IntervalTimer
  myTimer.begin(serviceUS, 1);  // serviceMS to run every 0.000001 seconds

  // Audio connections require memory to work. For more  
  // detailed information, see the MemoryAndCpuUsage example  
  AudioMemory(7);

  waveform1.begin(.1, 82.4, WAVEFORM_SAWTOOTH);
  waveform2.begin(.1, 82, WAVEFORM_SQUARE);
  waveform3.begin(.1, 81.7, WAVEFORM_SAWTOOTH);
  waveform4.begin(.05, 164, WAVEFORM_SINE);
  filter1.octaveControl(7);
  filter1.frequency(16000);


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
    digitalWrite( LEDPIN, digitalRead( LEDPIN ) ^ 0x01 );
  }
  if(usTimerB.flagStatus() == PENDING)
  {
    //User code
    //filter1.frequency(filterfreq);
    if( filterfreq < 2 )
    {
      filterfreq = 10000;
    }
    filterfreq = filterfreq * .9999;
  }
  if(usTimerC.flagStatus() == PENDING)
  {
    //User code
    filter1.resonance(filterq);
    filterq += 0.01;
    if( filterq > 5 )
    {
      filterq = 0.8;
    }
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



