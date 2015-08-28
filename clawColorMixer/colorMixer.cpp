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
#include "colorMixer.h"
#include <Adafruit_NeoPixel.h>

//**********************************************************************//
//
//  Color Mixer
//
//**********************************************************************//
ColorMixer::ColorMixer(uint16_t n, uint8_t p, uint8_t t) : Adafruit_NeoPixel(n, p, t)
{
  for (int i = 0; i < 8; i++)
  {
    mainPage[i].red = 0;
    mainPage[i].green = 0;
    mainPage[i].blue = 0;
    mainPage[i].alpha = 0;
  }
 
}

void ColorMixer::clearPage( void )
{
  for (int i = 0; i < 8; i++)
  {
    mainPage[i].red = 0;
    mainPage[i].green = 0;
    mainPage[i].blue = 0;
    mainPage[i].alpha = 0;
  }

}

void ColorMixer::addLayer( RGBA8_t inputColor )
{
  for (int i = 0; i < 8; i++)
  {

  }

}

void ColorMixer::addLayer( RGBA8_t * inputColorMatrix )
{
  for (int i = 0; i < 8; i++)
  {
    mainPage[i].red = inputColorMatrix[i].red;
    mainPage[i].green = inputColorMatrix[i].green;
    mainPage[i].blue = inputColorMatrix[i].blue;
  }

}
void ColorMixer::orLayer( RGBA8_t inputColor )
{
  for (int i = 0; i < 8; i++)
  {
    if( inputColor.red > mainPage[i].red )
    {
      mainPage[i].red =  inputColor.red;
    }
    if( inputColor.green > mainPage[i].green )
    {
      mainPage[i].green =  inputColor.green;
    }
    if( inputColor.blue > mainPage[i].blue )
    {
      mainPage[i].blue =  inputColor.blue;
    }
  }

}

void ColorMixer::mix( void )
{
  for (int i = 0; i < 8; i++)
  {
    setPixelColor(i, mainPage[i].red, mainPage[i].green, mainPage[i].blue);

  }

}
