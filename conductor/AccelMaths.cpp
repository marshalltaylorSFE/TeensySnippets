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

}

float AccelMaths::scaledXA( void )
{
 
  return XA;
}

float AccelMaths::scaledXV( void )
{
 
  return XV;
}

float AccelMaths::scaledXX( void )
{
  
  return XX;
}
