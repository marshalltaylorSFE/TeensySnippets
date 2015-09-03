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

//Looking out at arm,
// -x<--->x
//
//     y
//     |
//    -y
//
//     z
//    -Z (points away from you)


CircularBuffer xDataBuffer(32);
CircularBuffer xAverageBuffer(32);
CircularBuffer yDataBuffer(32);
CircularBuffer yAverageBuffer(32);
CircularBuffer zDataBuffer(32);
CircularBuffer zAverageBuffer(32);

//**********************************************************************//
//
//  Accelerometer maths
//
//**********************************************************************//
AccelMaths::AccelMaths( uint8_t busType, uint8_t inputArg ) : LSM6DS3( busType, inputArg )
{
  //LSM6DS3 myIMU;

  
}

void AccelMaths::tick( void )
{
  //********Update to the Buffers********//
  //Start by filling a circular buffer
  yDataBuffer.write(readFloatAccelY());
  xDataBuffer.write(readFloatAccelZ());
  zDataBuffer.write(readFloatAccelX());
  
  //Now average the circular buffer into another
  float floatTemp = 0;
  for( int i = 0; i < 30; i++)
  {
    floatTemp += xDataBuffer.read(i);
  }
  xAverageBuffer.write(floatTemp / 30);
  
  floatTemp = 0;
  for( int i = 0; i < 30; i++)
  {
    floatTemp += yDataBuffer.read(i);
  }
  yAverageBuffer.write(floatTemp / 30);
  
  floatTemp = 0;
  for( int i = 0; i < 30; i++)
  {
    floatTemp += zDataBuffer.read(i);
  }
  zAverageBuffer.write(floatTemp / 30);
  
}

//X
float AccelMaths::xMilliDeltaAverage( void )
{
	float temp = (xAverageBuffer.read(0) - xAverageBuffer.read(1) ) * 1000;
  return temp;
}

float AccelMaths::xRollingAverage( void )
{
  return xAverageBuffer.read(0);
}

//Y
float AccelMaths::yMilliDeltaAverage( void )
{
	float temp = (yAverageBuffer.read(0) - yAverageBuffer.read(1) ) * 1000;
  return temp;
}

float AccelMaths::yRollingAverage( void )
{
  return yAverageBuffer.read(0);
}

//Z
float AccelMaths::zMilliDeltaAverage( void )
{
	float temp = (zAverageBuffer.read(0) - zAverageBuffer.read(1) ) * 1000;
  return temp;
}

float AccelMaths::zRollingAverage( void )
{
  return zAverageBuffer.read(0);
}
