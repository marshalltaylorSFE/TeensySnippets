//**********************************************************************//
//  BEERWARE LICENSE
//
//  This code is free for any use provided that if you meet the author
//  in person, you buy them a beer.
//
//  This license block is BeerWare itself.
//
//  Written by:  Marshall Taylor
//  Created:  May 26, 2015
//
//**********************************************************************//

//Includes
#include "accelMaths.h"
#include "SparkFunLSM6DS3.h"
#include "Arduino.h"
#include "CircularBuffer.h"

CircularBuffer upDataBuffer(32);
CircularBuffer upAverageBuffer(32);
CircularBuffer rightDataBuffer(32);
CircularBuffer rightAverageBuffer(32);
CircularBuffer outDataBuffer(32);
CircularBuffer outAverageBuffer(32);

CircularBuffer verticalDerivativeBuffer(32);



//**********************************************************************//
//
//  Accelerometer maths
//
//**********************************************************************//
AccelMaths::AccelMaths( uint8_t busType, uint8_t inputArg ) : LSM6DS3( busType, inputArg )
{
  //LSM6DS3 myIMU;
  msDeltaT = 1;
  
}

void AccelMaths::tick( void )
{
  //********Update to the Buffers********//
  //Start by filling a circular buffer
  upDataBuffer.write(readFloatAccelY());
  rightDataBuffer.write(readFloatAccelZ());
  //outDataBuffer.write(readFloatAccelX());
  
  //Now average the circular buffer into another
  float floatTemp = 0;
  for( int i = 0; i < 30; i++)
  {
    floatTemp += upDataBuffer.read(i);
  }
  upAverageBuffer.write(floatTemp / 30);
  
  floatTemp = 0;
  for( int i = 0; i < 30; i++)
  {
    floatTemp += rightDataBuffer.read(i);
  }
  rightAverageBuffer.write(floatTemp / 30);
  
  // floatTemp = 0;
  // for( int i = 0; i < 30; i++)
  // {
    // floatTemp += outDataBuffer.read(i);
  // }
  // outAverageBuffer.write(floatTemp / 30);
  
  //********Do Buffer Specific Calculation********//
  //Newer minus older
    // Serial.print("\n");
	// Serial.print(upAverageBuffer.read(0), 4);
	// Serial.print(",");
	// Serial.print(upAverageBuffer.read(1), 4);
  verticalDerivativeBuffer.write( (upAverageBuffer.read(0) - upAverageBuffer.read(1) ) * 1000);
  
}

float AccelMaths::milliDeltaAverageUp( void )
{
  return verticalDerivativeBuffer.read(0);
}

float AccelMaths::milliDeltaDeltaAverageUp( void )
{
  float returnValue = (verticalDerivativeBuffer.read(0) - verticalDerivativeBuffer.read(1));
  return returnValue;
}

float AccelMaths::rollingAverageUp( void )
{
  return upAverageBuffer.read(0);
}

float AccelMaths::rollingAverageRight( void )
{
  return rightAverageBuffer.read(0);
}
