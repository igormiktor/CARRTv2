/*
    PauseState.cpp - Pause state for CARRT

    Copyright (c) 2022 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/





#include "PauseState.h"

#include <avr/pgmspace.h>

#include "ErrorCodes.h"
#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/Motors.h"



#ifndef CARRT_SHOW_PAUSE_IN_DISPLAY
    #define CARRT_SHOW_PAUSE_IN_DISPLAY     0
#endif


PauseState::PauseState() :
mPausedState( 0 )
{
}


void PauseState::storePausedState( State* pausedState )
{
    mPausedState = pausedState;
}


State* PauseState::transferPausedState()
{
    State* pausedState = mPausedState;
    mPausedState = 0;

    return pausedState;
}


void PauseState::onEntry()
{
    // Choice to not display pause msg (resetting display when continuing would be a lot of code change)
#if CARRT_SHOW_PAUSE_IN_DISPLAY
    Display::clear();               //0123456789012345
    Display::displayTopRowP16( PSTR( "***  Paused  ***" ) );
#endif // CARRT_SHOW_PAUSE_IN_DISPLAY

    return;
}


void PauseState::onExit()
{
    // Never delete this event
}


bool PauseState::onEvent( uint8_t event, int16_t param )
{
    return true;       
}


