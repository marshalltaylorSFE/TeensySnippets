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
#ifndef ACTIONMACHINES_H_INCLUDED
#define ACTIONMACHINES_H_INCLUDED

#include "stdint.h"
#include "timeKeeper.h"

enum MMStates {
  MMIdle,
  MMRightTickTimingState,
  MMRightTickActionState,
  MMLeftTickTimingState,
  MMLeftTickActionState
  

};

//#define DBTICKMS 10;

class MainMachine
{
public:

  float rightIn;
  float upIn;
  float outIn;
  
  uint8_t serialOutputEnable;
  
  TimeKeeper tickTimer;
  TimeKeeper tickTimerHoldOff;
  
  MMStates state;

  MainMachine( void );
  void tick( void );

};

enum BSMStates {
  BSMIdle,
  BSMDownBeatFallingState,
  BSMDownBeatTimingRiseState,
  BSMDownBeatHoldOffState,
  
};

//#define DBTICKMS 10;

class BeatStateMachine
{
public:

  float upIn;
  float upDeltaIn;
  
  uint8_t startFlag;
  uint8_t exitFlag;
  uint8_t risingFlag;
  uint8_t servicedFlag;
  
  uint16_t lastTime;
  
  TimeKeeper tickTimer;
  
  BSMStates state;

  BeatStateMachine( void );
  void tick( void );

};

#endif