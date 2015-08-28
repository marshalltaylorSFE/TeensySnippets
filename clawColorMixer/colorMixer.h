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
#ifndef COLORMIXER_H_INCLUDED
#define COLORMIXER_H_INCLUDED

#include "stdint.h"
#include <Adafruit_NeoPixel.h>

//Structures
struct RGBA8_t
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
};

//Classes
class ColorMixer : public Adafruit_NeoPixel
{
public:
    ColorMixer(uint16_t n, uint8_t p=6, uint8_t t=NEO_GRB + NEO_KHZ800);// : Adafruit_NeoPixel(n, p, t);
    void clearPage( void );
    void addLayer( RGBA8_t );//Color, opacity
    void addLayer( RGBA8_t * );//Color, opacity
    void orLayer( RGBA8_t );//Color, opacity
    void mix( void );

    RGBA8_t mainPage[8];
	
private:

};


#endif // FILE_H_INCLUDED

