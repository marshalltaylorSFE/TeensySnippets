// force = mass * acceleration,
// force = nibbleweights * acceleration
// where
//   [force] = nibbleweight * bits / s^2
//   [mass] = 1/16*nibbleweight
//   ???
//   Profit!
#include "stdint.h"

class ColorPoint
{
public:

  uint8_t red;
  uint8_t green;
  uint8_t blue;
  float redF;
  float greenF;
  float blueF;
  float redP;
  float greenP;
  float blueP;
  float redV;
  float greenV;
  float blueV;

  uint8_t damping;
  uint8_t mass;
  
  ColorPoint( void ); 
  void tick( void );
};

