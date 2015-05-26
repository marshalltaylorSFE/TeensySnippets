#ifndef COLORTOOLS_H_INCLUDED
#define COLORTOOLS_H_INCLUDED

#include "stdint.h"

enum FDStates {
  FDidle,
  FDrampUp,
  FDholdOn,
  FDrampDown,
  FDholdOff

};

class FlashDialog
{
public:

  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t messageNumber;
  uint8_t enabled;
  uint16_t counter;

  FDStates state;

  FlashDialog( void );
  void enable( uint8_t );
  void message( uint8_t );
  void tick( void );

};

enum DBStates {
  DBidle,
  DBrampUp,
  DBholdOn,
  DBrampDown

};

class DownBeat
{
public:

  uint8_t red;
  uint8_t green;
  uint8_t blue;

  uint8_t enabled;
  uint16_t counter;

  DBStates state;

  DownBeat( void );
  void enable( uint8_t );
  void tick( void );

};

class TimeKeeper
{
public:
    TimeKeeper( void );
    void mClear( void );
    uint16_t mGet( void );
    void mIncrement( uint8_t );

private:
    uint16_t timeElapsed;

};

#endif
