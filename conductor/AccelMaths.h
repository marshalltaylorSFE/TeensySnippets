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

//Classes
class AccelMaths : public LSM6DS3
{
public:
    AccelMaths( void );
	void tick( void );
	float scaledXA( void );
	float scaledXV( void );
	float scaledXX( void );
	
private:
	int16_t msDeltaT;
	float vDecay;
	float xDecay;
	float gOffset;
	float lastXA;
	float lastXV;
        float lastXX;
	float XA;
	float XV;
        float XX;


};


#endif

