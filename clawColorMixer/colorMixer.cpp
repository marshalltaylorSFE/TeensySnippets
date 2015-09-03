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

RGBA8::RGBA8( void )
{
	red = 0;
	green = 0;
	blue = 0;
	alpha = 0;
	
}

void RGBA8::clear( void )
{
	red = 0;
	green = 0;
	blue = 0;
	alpha = 0;
	
}

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

void ColorMixer::addLayer( RGBA8 inputColor )
{
  for (int i = 0; i < 8; i++)
  {
    mainPage[i].red = (((uint16_t)inputColor.red * (uint16_t)inputColor.alpha) >> 8 ) + (((uint16_t)mainPage[i].red * ( (uint16_t)255 - inputColor.alpha)) >> 8 );
    mainPage[i].green = (((uint16_t)inputColor.green * (uint16_t)inputColor.alpha) >> 8 ) + (((uint16_t)mainPage[i].green * ( (uint16_t)255 - inputColor.alpha)) >> 8 );
    mainPage[i].blue = (((uint16_t)inputColor.blue * (uint16_t)inputColor.alpha) >> 8 ) + (((uint16_t)mainPage[i].blue * ( (uint16_t)255 - inputColor.alpha)) >> 8 );
  }

}

void ColorMixer::addLayer( RGBA8 * inputColorMatrix )
{
  for (int i = 0; i < 8; i++)
  {
    mainPage[i].red = (((uint16_t)inputColorMatrix[i].red * (uint16_t)inputColorMatrix[i].alpha) >> 8 ) + (((uint16_t)mainPage[i].red * ( (uint16_t)255 - inputColorMatrix[i].alpha)) >> 8 );
    mainPage[i].green = (((uint16_t)inputColorMatrix[i].green * (uint16_t)inputColorMatrix[i].alpha) >> 8 ) + (((uint16_t)mainPage[i].green * ( (uint16_t)255 - inputColorMatrix[i].alpha)) >> 8 );
    mainPage[i].blue = (((uint16_t)inputColorMatrix[i].blue * (uint16_t)inputColorMatrix[i].alpha) >> 8 ) + (((uint16_t)mainPage[i].blue * ( (uint16_t)255 - inputColorMatrix[i].alpha)) >> 8 );
  }

}
void ColorMixer::orLayer( RGBA8 inputColor )
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

void ColorMixer::gradientAddLayer( RGBA8 point1Color, int16_t point1Position, RGBA8 point2Color, int16_t point2Position )
{
	//sanitize
	if( point1Position > 20 )
	{
		point1Position = 20;
	}
	if( point1Position < -20 )
	{
		point1Position = -20;
	}
	if( point2Position > 20 )
	{
		point2Position = 20;
	}
	if( point2Position < -20 )
	{
		point2Position = -20;
	}
	//Make sure position two is always greater than position 1
	if( point1Position > point2Position ) //then swap them
	{
		int16_t swapPosition = point2Position;
		point2Position = point1Position;
		point1Position = swapPosition;
		RGBA8 swapColor = point2Color;
		point2Color = point1Color;
		point1Color = swapColor;
	}
	
	//Temporary matrix
	RGBA8 outField[8];
	

	//Calc line for alpha
	int16_t slope = ((int16_t)point2Color.alpha - (int16_t)point1Color.alpha ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i <= 7 ) )
		{
			outField[i].alpha = point1Color.alpha + ((i - point1Position) * slope);
		}
	}

	//Calc line for red
	slope = ((int16_t)point2Color.red - (int16_t)point1Color.red ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i <= 7 ) )
		{
			outField[i].red = point1Color.red + ((i - point1Position) * slope);
		}
	}

	//Calc line for green
	slope = ((int16_t)point2Color.green - (int16_t)point1Color.green ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i <= 7 ) )
		{
			outField[i].green = point1Color.green + ((i - point1Position) * slope);
		}
	}

	//Calc line for blue
	slope = ((int16_t)point2Color.blue - (int16_t)point1Color.blue ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i <= 7 ) )
		{
		outField[i].blue = point1Color.blue + ((i - point1Position) * slope);
		}
	}
	
	//Call the mixer
	addLayer(outField);
	
	
}

void ColorMixer::brush( int8_t position, RGBA8 color, int8_t sideWall )
{
	//Draw two lines
	RGBA8 sidePixel;
	sidePixel = color;
	sidePixel.alpha = sidePixel.alpha >> 1;
	
	gradientAddLayer( color, position, sidePixel, position + sideWall );
	
	gradientAddLayer( color, position, sidePixel, position - sideWall );
	
	
}

void ColorMixer::rotate( int8_t offset )
{
	//Temporary matrix
	RGBA8 outField[8];
	//Temp offset
	int8_t offsetVector = 0;
	
	for( int i = 0; i < 8; i++ )
	{
		//roll over
		offsetVector = i + offset;
		while( offsetVector > 7 )
		{
			offsetVector = offsetVector - 8;
		}
		while( offsetVector < 0 )
		{
			offsetVector = offsetVector + 8;
		}

		outField[i] = mainPage[offsetVector];
		outField[i].alpha = 255;
	}
	
	//Call the mixer
	addLayer(outField);
}

void ColorMixer::mix( void )
{
  for (int i = 0; i < 8; i++)
  {
    setPixelColor(i, mainPage[i].red, mainPage[i].green, mainPage[i].blue);

  }

}
