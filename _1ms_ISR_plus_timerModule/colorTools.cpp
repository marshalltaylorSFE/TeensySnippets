#include "colorTools.h"

ColorPoint::ColorPoint( void )
{
  redF = 0;
  greenF = 0;
  blueF = 0;
  redP = 0;
  greenP = 0;
  blueP = 0;
  redV = 0;
  greenV = 0;
  blueV = 0;

  damping = 0;
  mass = 1;
  
}

void ColorPoint::tick( void )
{
  float tempRedP = redP + redV * 0.001;
  float tempRedV = redV + ( (redF / mass) * 0.001 )/2;
  if(( tempRedP < 127 )&&( tempRedP > -127))
  {
    redP = tempRedP;
  }
  if(( tempRedV < 100 )&&( tempRedV > -100))
  {
    redV = tempRedV;
  }
  
}

