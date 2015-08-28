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

#include "actionMachines.h"
#include "timeKeeper.h"

#define TICKTHRESHOLD 1.5
#define TICKDELAY 50
#define TICKHOLDOFF 200

//****************************************************************************//
//
//  Main Machine
//
//****************************************************************************//
MainMachine::MainMachine( void )
{
  rightIn = 0;
  upIn = 0;
  outIn = 0;
  
  serialOutputEnable = 0;
  
  state = MMIdle;

}

void MainMachine::tick( void )
{
	//Tend to the time keeper
	tickTimer.mIncrement( 2 );
	tickTimerHoldOff.mIncrement( 2 );

    //Now do the states.
    MMStates nextState = state;
    switch( state )
    {
	//***** MAIN IDLE *****//
    case MMIdle:
        if( rightIn > TICKTHRESHOLD )
        {
		    tickTimer.mClear();
            nextState = MMRightTickTimingState;
        }
        if( rightIn < ( TICKTHRESHOLD * -1 ) )
        {
		    tickTimer.mClear();
            nextState = MMLeftTickTimingState;
        }
        break;
	//***** START OF RIGHT TICK BRANCH *****//
	case MMRightTickTimingState:
        if( rightIn > TICKTHRESHOLD )
        {
			if( tickTimer.mGet() > TICKDELAY )
			{
				//Timer expired
				//Start hold off timer
				tickTimerHoldOff.mClear();
				nextState = MMRightTickActionState;
			}
			else
			{
				//Timer not expired
				nextState = state;  //Stay where you are
			}
        }
        else
        {
            //Find your home!
			nextState = MMIdle;
        }
        break;
	case MMRightTickActionState:
		//Do your thing
		//Enable serial output
		serialOutputEnable = 1;
		if( tickTimerHoldOff.mGet() > TICKHOLDOFF )
		{
			nextState = MMIdle;
		}
		break;
	//***** START OF LEFT TICK BRANCH *****//
	case MMLeftTickTimingState:
        if( rightIn < ( TICKTHRESHOLD * -1 ) )
        {
			if( tickTimer.mGet() > TICKDELAY )
			{
				//Timer expired
				//Start holdoff timer
				tickTimerHoldOff.mClear();
				nextState = MMLeftTickActionState;
			}
			else
			{
				//Timer not expired
				nextState = state;  //Stay where you are
			}
        }
        else
        {
            //Find your home!
			nextState = MMIdle;
        }
        break;
	case MMLeftTickActionState:
		//Do your thing
		//Disable serial output
		serialOutputEnable = 0;
		if( tickTimerHoldOff.mGet() > TICKHOLDOFF )
		{
			nextState = MMIdle;
		}
		break;
    default:
        nextState = MMIdle;
        break;
    }
    state = nextState;

}

//#define DBTICKMS 10;


#define DOWNBEATLOWERTHRESHOLD -65
#define DOWNBEATUPPERTHRESHOLD 30
#define DOWNBEATRISEFAILLIMIT 1200 //Max, fail limit
#define DOWNBEATHOLDOFF 100

//****************************************************************************//
//
//  Beat State Machine
//
//  This machine processes how a downbeat is detected.
//  It takes data in through upIn and upDeltaIn.
//
//****************************************************************************//
BeatStateMachine::BeatStateMachine( void )
{
  upIn = 0;
  upDeltaIn = 0;
  
  startFlag = 0;
  exitFlag = 0;
  risingFlag = 0;
  servicedFlag = 0;
  
  state = BSMIdle;

}
  
void BeatStateMachine::tick( void )
{
	//Tend to the time keeper
	tickTimer.mIncrement( 2 );

    //Now do the states.
    BSMStates nextState = state;
    switch( state )
    {
	//***** MAIN IDLE *****//
    case BSMIdle:
        if( upIn < DOWNBEATLOWERTHRESHOLD )
        {
			startFlag = 1;
            nextState = BSMDownBeatFallingState;
        }
        break;
	case BSMDownBeatFallingState:
        if( upDeltaIn < 0 ) //Slope is negative
        {
			//Do nothing
		}
		else
		{
			//Leave
			risingFlag = 1;
			tickTimer.mClear();
			nextState = BSMDownBeatTimingRiseState;
        }
        break;
	case BSMDownBeatTimingRiseState:
        if( upIn > DOWNBEATUPPERTHRESHOLD )
        {
		    //Record time
			lastTime = tickTimer.mGet();
			
			//Clear timer
			tickTimer.mClear();
			
			//lower flag
			risingFlag = 0;
			
			//Leave
			exitFlag = 1;
			nextState = BSMDownBeatHoldOffState;
        }
		if( tickTimer.mGet() > DOWNBEATRISEFAILLIMIT )
		{
		    //Something went wrong
		    startFlag = 0;
		    exitFlag = 0;
			risingFlag = 0;
			
		    nextState = BSMIdle;
		}
        break;
	case BSMDownBeatHoldOffState:
		//Do your thing
		if( tickTimer.mGet() > TICKHOLDOFF )
		{
		  if( servicedFlag == 1 )
		  {
		    startFlag = 0;
		    exitFlag = 0;
			servicedFlag = 0;
			nextState = BSMIdle;
		  }
		}
		break;
    default:
        nextState = BSMIdle;
        break;
    }
    state = nextState;

}
