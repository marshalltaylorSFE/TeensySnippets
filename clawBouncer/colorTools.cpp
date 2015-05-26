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
  float tempRedP = redP + redV * 0.1 + 0.5 * (redF / mass) * .01;
  float tempRedV = redV + ( (redF / mass) * 0.1 );
  tempRedV = tempRedV * .90;
  if((tempRedP < 32000 )&&( tempRedP >= 16000 ))
  {
    redP = tempRedP;
    red = 255 - ( 255 * ( tempRedP - 16000 ) / 16000);
    blue = 0;
  }
  if(( tempRedP < 16000 )&&( tempRedP >= 0 ))
  {
    redP = tempRedP;
    red = 255 * tempRedP / 16000;
    blue = 255 - red;
  }
  if( tempRedP < 0 )
  {
    redV = -tempRedV;
  }
  else
  {
    redV = tempRedV;
  }
  
}

