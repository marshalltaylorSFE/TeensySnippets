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
#ifndef ACCELMATHS_H_INCLUDED
#define ACCELMATHX_H_INCLUDED

#include "stdint.h"
#include "SparkFunLSM6DS3.h"

struct PSlice
{
	float accel = 0;
	float velo = 0;
    float deflect = 0;
	
};

//Classes
class AccelMaths : public LSM6DS3
{
public: //B(int a) : A(a)
  //AccelMaths( uint8_t busType ) : LSM6DS3( busType );
  AccelMaths( uint8_t, uint8_t );// : LSM6DS3( uint8_t, uint8_t );
	void tick( void );

	float debugAverage;
	float debugAverageZ;	
	float milliDeltaAverageUp( void );
	float milliDeltaDeltaAverageUp( void );
	float rollingAverageUp( void );
	float rollingAverageRight( void );
private:
	int16_t msDeltaT;

};


#endif
