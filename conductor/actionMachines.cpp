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

#define TICKTHRESHOLD 0.5
#define TICKDELAY 150
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
        if( rightIn < ( TICKTHRESHOLD * -1 ) )
        {
		    tickTimer.mClear();
            nextState = MMRightTickTimingState;
        }
        if( rightIn > TICKTHRESHOLD )//( TICKTHRESHOLD * -1 ) )
        {
		    tickTimer.mClear();
            nextState = MMLeftTickTimingState;
        }	//***** START OF RIGHT TICK BRANCH *****//
        break;
	case MMRightTickTimingState:
        if( rightIn < ( TICKTHRESHOLD * -1 ) )
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
        if( rightIn > TICKTHRESHOLD )//( TICKTHRESHOLD * (float)-1 ) )
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
