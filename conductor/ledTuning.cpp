#include "ledTuning.h"
#include "stdint.h"
#include "math.h"
//#include "arduino.h"

//A class for converting visible brightness to liner uint8_t values

// pass factor while constructing
LedTable::LedTable( float input )
{
  factor = input;
}

//Call calcTable() while defining to do the work of making the LUT
void LedTable::calcTable( void )
{
  for( int i = 0; i < 256; i++ )
  {
    ledLUT[i] = (pow(((float)i/256), factor)) * 255;
  }
}

// use ledAdjust to convert 0-255 numbers to 0-255 numbers with nonliner weighting
uint8_t LedTable::ledAdjust( uint8_t input )
{
  return ledLUT[input];
}


