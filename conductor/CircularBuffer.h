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
#ifndef CIRCULARBUFFER_H_INCLUDED
#define CIRCULARBUFFER_H_INCLUDED

#define CIRCULAR_BUFFER_MAX_LENGTH 255

#include "stdint.h"


//Classes
class CircularBuffer
{
public:
    CircularBuffer( uint8_t depthInput );
	
	float read( uint8_t );
	void write( float inputData );
	void clear( void );

	private:
	uint8_t topPointer;
	uint8_t bytesUsed;
	uint8_t bufferDepth;
	float buffer[CIRCULAR_BUFFER_MAX_LENGTH];	
};


#endif
