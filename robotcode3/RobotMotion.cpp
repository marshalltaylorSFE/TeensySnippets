//**RobotMotion*******************************//
#include "RobotMotion.h"
//#include "PanelComponents.h"
#include "Panel.h"
#include "Arduino.h"
#include "flagMessaging.h"

#define KNOBDELTADETECT 5  //5 units of 255 for knob change detect
#define ACCEL_RATE 0.1
#define DECEL_RATE 0.02
#define BK_HOLDOFFMS 250

RobotMotion::RobotMotion( void )
{
	//Controls
	state = PInit;
	velocity = 0;
	velocityShadow = 0;
	direction = 0;
	frontSwap = 0;
	bkupHoldTimeKeeper.mClear();
	
}

void RobotMotion::reset( void )
{
	//Set explicit states
	
	update();
	
}

//---------------------------------------------------------------------------//
//
//  To process the machine,
//    take the inputs from the system
//    process human interaction hard-codes
//    process the state machine
//    clean up and post output data
//
//---------------------------------------------------------------------------//
void RobotMotion::processMachine( void )
{
	
	
	//Do main machine
	tickStateMachine();
	
	update();
}

void RobotMotion::tickStateMachine()
{
	//***** PROCESS THE LOGIC *****//
	if( bButton.serviceRisingEdge() )
	{
		frontSwap ^= 0x01;
	}
    //Now do the states.
    PStates nextState = state;
    switch( state )
    {
    case PInit:
		nextState = PIdle;
		break;
	case PIdle:
		if( rightButton.getState() )
		{
			direction = 1;
		}
		else if( leftButton.getState() )
		{
			direction = -1;
		}
		else
		{
			direction = 0;
		}
		if( downButton.serviceRisingEdge() )
		{
			nextState = PBackward;
		}
		if( upButton.serviceRisingEdge() )
		{
			nextState = PForward;
		}
		if( aButton.getState() )
		{
			velocity = 1;
			nextState = PForward;
		}
        break;
	case PSleepy:
		nextState = PIdle; //Default operation
		break;
	case PForward:
		if( aButton.getState() )
		{
			velocity = 1;
		}
		if( upButton.getState() )
		{
			//increase velo
			velocity += ACCEL_RATE;
			if( velocity > 1 )
			{
				//cap it
				velocity = 1;
			}
		}
		else
		{
			//decrease velo
			velocity -= DECEL_RATE;
			if( velocity < 0 )
			{
				//cap it
				velocity = 0;
			}
		}		
		if( velocity == 0)
		{
			nextState = PIdle; //Default operation
		}
		if ( rightButton.getState() )
		{
			direction = 0.5;
		}
		if ( leftButton.getState() )
		{
			direction = -0.5;
		}
		if( downButton.serviceRisingEdge() )
		{
			//Brake
			velocity = 0;
			direction = 0;
			//Start backup timer
			velocityShadow = 0;
			bkupHoldTimeKeeper.mClear();
			nextState = PStop;
		}
        break;
	case PStop:
		if( (downButton.getState()) && ( bkupHoldTimeKeeper.mGet() < BK_HOLDOFFMS ))
		{
			//being held
			velocityShadow -= DECEL_RATE;
			if( velocityShadow < -1 )
			{
				//cap it
				velocityShadow = -1;
			}
		}
		else
		{
			if(downButton.getState())
			{
				//button still held
				//Set the shadow and depart
				velocity = velocityShadow;
				nextState = PBackward;
			}
			else
			{
				nextState = PIdle;
			}

		}
        break;
	case PBackward:
		if( aButton.getState() )
		{
			velocity = 1;
			nextState = PForward;
		}
		if( downButton.getState() )
		{
			//decrease velo
			velocity -= ACCEL_RATE;
			if( velocity < -1 )
			{
				//cap it
				velocity = -1;
			}
		}
		else
		{
			//increase velo
			velocity += DECEL_RATE;
			if( velocity > 0 )
			{
				//cap it
				velocity = 0;
			}
		}		
		if( velocity == 0)
		{
			nextState = PIdle; //Default operation
		}
		if ( rightButton.getState() )
		{
			direction = 0.5;
		}
		if ( leftButton.getState() )
		{
			direction = -0.5;
		}
		if( upButton.serviceRisingEdge() )
		{
			//Brake
			velocity = 0;
			direction = 0;
			nextState = PForward;
		}
        break;
	default:
		nextState = PIdle; //Default operation
        break;
    }

    state = nextState;

}

void RobotMotion::timersMIncrement( uint8_t inputValue )
{
	leftButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	rightButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	upButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	downButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	selectButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	startButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	aButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	bButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	bkupHoldTimeKeeper.mIncrement(inputValue);

}