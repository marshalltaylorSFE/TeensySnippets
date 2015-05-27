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

//**********************************************************************//
//
//  Color Mixer
//
//**********************************************************************//
AccelMaths::AccelMaths( void )
{
  //LSM6DS3 myIMU;
  msDeltaT = 10;
  vDecay = 0.9998;
  xDecay = 0.9998;
  gOffset = 0.98;
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
  
  XA = readFloatAccelY() + gOffset;
  XV = (lastXV + ((float)msDeltaT / 10) * (XA))*vDecay;
  XX = (lastXX + ((float)msDeltaT / 10) * (XV))*xDecay;

}

float AccelMaths::scaledXV( void )
{
 
  return XV;
}

float AccelMaths::scaledXX( void )
{
  
  return XA;
}
