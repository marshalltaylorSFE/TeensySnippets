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

#include "CircularBuffer.h"

CircularBuffer myBuffer(50);
CircularBuffer myBufferZ(50);

//**********************************************************************//
//
//  Accelerometer maths
//
//**********************************************************************//
AccelMaths::AccelMaths( void )
{
  //LSM6DS3 myIMU;
  msDeltaT = 1;
  vDecay = 1;
  xDecay = 1;
  gOffset = 1;
  lastXA = 0;
  lastXV = 0;
  lastXX = 0;
  XV = 0;
  XX = 0;
  XA = 0;
  
}

void AccelMaths::tick( void )
{

  
  //Save the last point
  lastXX = XX;
  lastXV = XV;
  lastXA = XA;
  
  XA = (-1 * (readFloatAccelY() + gOffset))*3;
  myBuffer.write(readFloatAccelY());
  myBufferZ.write(readFloatAccelZ());

  if(( XA > -0.05 )&&( XA < 0.05 ))
  {
    XA = 0;
    XV = 0;
    lastXV= 0;
  }
  if(XX > 500)
  {
    lastXX = 500;
    XV = 0;
    lastXV= 0;
  }  
  if(XX < -500)
  {
    lastXX = -500;
    XV = 0;
    lastXV= 0;
  }  
  XV = (lastXV + ((float)msDeltaT) * (XA)) * vDecay;
  XX = (lastXX + ((float)msDeltaT) * (XV)) * xDecay;
  
  //Had enough?  Didn't think so.  Do more math
  debugAverage = 0;
  for( int i = 0; i < 40; i++)
  {
    debugAverage += myBuffer.read(i);
  }
  debugAverage = debugAverage / 40;
  debugAverageZ = 0;
  for( int i = 0; i < 40; i++)
  {
    debugAverageZ += myBufferZ.read(i);
  }
  debugAverageZ = debugAverageZ / 40;  

}

float AccelMaths::scaledXA( void )
{
 
  return XA;
}

float AccelMaths::rollingAverage( void )
{
  return debugAverage;
}
float AccelMaths::rollingAverageZ( void )
{
  return debugAverageZ;
}
float AccelMaths::scaledXV( void )
{
 
  return XV;
}

float AccelMaths::scaledXX( void )
{
  
  return XX;
}
