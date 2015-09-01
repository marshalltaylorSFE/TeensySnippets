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
#ifndef COLORMACHINES_H_INCLUDED
#define COLORMACHINES_H_INCLUDED

#include "stdint.h"

enum FDStates {
  FDidle,
  FDrampUp,
  FDholdOn,
  FDrampDown,
  FDholdOff

};

#define FDTICKMS 10;

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

#define DBTICKMS 10;

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

#endif