#include "stdint.h"

//A class for converting visible brightness to liner uint8_t values

class LedTable
{
  public:
    // pass factor while constructing
    LedTable( float input );
    
    //Call calcTable() while defining to do the work of making the LUT
    void calcTable( void );
    
    // use ledAdjust to convert 0-255 numbers to 0-255 numbers with nonliner weighting
    uint8_t ledAdjust( uint8_t input );

  private:
    //Non-liner factor
    float factor;
    //data containing the converted values
    uint8_t ledLUT[256];
    
};
