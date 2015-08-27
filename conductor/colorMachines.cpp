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

#include "colorMachines.h"
#include "timeKeeper.h"

//****************************************************************************//
//
//  Flasher
//
//****************************************************************************//
FlashDialog::FlashDialog( void )
{
  red = 0;
  green = 0;
  blue = 0;
  messageNumber = 0;
  enabled = 0;
  counter = 0;

  state = FDidle;

}

void FlashDialog::enable( uint8_t inputEnable )
{
    //Set enable state
    enabled = inputEnable;

}

void FlashDialog::message( uint8_t inputNumber )
{
    //Choose what to display
    messageNumber = inputNumber;

}

void FlashDialog::tick( void )
{
    counter += 10; //10 ms per tick
    if(counter > 10000)
    {
        counter = 10000;
    }

    //Now do the states.
    FDStates nextState = state;
    switch( state )
    {
    case FDidle:
        if( enabled == 1 )
        {
            nextState = FDrampUp;
            counter = 0;
        }
        break;
    case FDrampUp:
        if( counter > 50)
        {
            nextState = FDholdOn;
            counter = 0;
        }
        else
        {
            red = 255 * (uint32_t)counter / 50;
        }
        break;
    case FDholdOn:
        if( counter >= 0)
        {
            nextState = FDrampDown;
            counter = 0;
        }
        else
        {
            red = 255;
        }
        break;
    case FDrampDown:
        if( counter > 50)
        {
            nextState = FDholdOff;
            counter = 0;
        }
        else
        {
            red = 255 - 255 * (uint32_t)counter / 50;
        }
        break;
    case FDholdOff:
        if( counter > 200)
        {
            nextState = FDidle;
            counter = 0;
        }
        else
        {
            red = 0;
        }
        break;
    default:
        nextState = FDidle;
        break;
    }
    state = nextState;

}

//****************************************************************************//
//
//  DownBeat
//
//****************************************************************************//
DownBeat::DownBeat( void )
{
  red = 0;
  green = 0;
  blue = 0;

  enabled = 0;
  counter = 0;

  state = DBidle;

}

void DownBeat::enable( uint8_t inputEnable )
{
    //Set enable state
    enabled = inputEnable;

}

void DownBeat::tick( void )
{
    counter += 10; //10 ms per tick
    if(counter > 10000)
    {
        counter = 10000;
    }

    //Now do the states.
    DBStates nextState = state;
    switch( state )
    {
    case DBidle:
        if( enabled == 1 )
        {
            nextState = DBrampUp;
            counter = 0;
        }
        break;
    case DBrampUp:
        if( counter >= 300)
        {
            nextState = DBholdOn;
            counter = 0;
        }
        else
        {
            green = 255 * (uint32_t)counter / 300;
        }
        break;
    case DBholdOn:
        if( counter > 40)
        {
            nextState = DBrampDown;
            counter = 0;
        }
        else
        {
            green = 255;
        }
        break;
    case DBrampDown:
        if( counter > 40)
        {
            nextState = DBidle;
            counter = 0;
        }
        else
        {
            green = 255 - 255 * (uint32_t)counter / 40;
        }
        break;
    default:
        nextState = DBidle;
        break;
    }
    state = nextState;

}
