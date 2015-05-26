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
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define REDPIN 9
#define GREENPIN 16
#define BLUEPIN 17

//Globals

LedTable myLedTable( 5 );

FlashDialog myFlashDialog;
DownBeat myDownBeat;

IntervalTimer myTimer;
LSM6DS3 myIMU;

TimerClass msTimerLED( 5 );
TimerClass msTimerStates( 10 );

uint16_t msTicks = 0;
uint8_t msTicksMutex = 1; //start locked out

#define MAXINTERVAL 2000
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

#define PIN 3

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

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
  
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

  myFlashDialog.enable(1);
  
}

int i = 0;
void loop()
{
  // main program
  if( msTicksMutex == 0 )  //Only touch the timers if clear to do so.
  {
    msTimerLED.update(msTicks);
    msTimerStates.update(msTicks);
    
    //Done?  Lock it back up
    msTicksMutex = 1;
  }  //The ISR should clear the mutex.
  
  if(msTimerLED.flagStatus() == PENDING)
  {
    if(( myFlashDialog.red > 0 )||( myDownBeat.green > 0))
    {
      for(int i=0;i<NUMPIXELS;i++)
      {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(myFlashDialog.red,myDownBeat.green,myFlashDialog.blue)); // Moderately bright green color.
        //pixels.setPixelColor(i, pixels.Color(0,green,0)); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.

      }
    }
    else
    {
      pixels.setPixelColor(0,50,0,0);
      pixels.setPixelColor(1,35,15,10);
      pixels.setPixelColor(2,10,15,20);
      pixels.setPixelColor(3,0,0,30);
      pixels.setPixelColor(4,0,0,30);
      pixels.setPixelColor(5,10,15,20);
      pixels.setPixelColor(6,35,15,10);
      pixels.setPixelColor(7,50,0,0);
      pixels.show();
    }
    
  }
  if(msTimerStates.flagStatus() == PENDING)
  {
    myFlashDialog.tick();
    myDownBeat.tick();

  }

  float yValue = myIMU.readFloatAccelY();
  float zValue = myIMU.readFloatAccelZ();

  //if ( yValue > 8 ) yValue = 8;
  if ( yValue < -4 )
  {
    myDownBeat.enable(1);
    myFlashDialog.enable(0);
  }
  else
  {
    myDownBeat.enable(0);
  }
  
  if(( zValue > 5 )&&( yValue > 3 ))
  {
    myFlashDialog.enable(1);
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


