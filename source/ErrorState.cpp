/*
    ErrorState.cpp - Error state for CARRT

    Copyright (c) 2016 Igor Mikolic-Torreira.  All right reserved.

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





#include "ErrorState.h"

#include "ErrorCodes.h"
#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/Motors.h"





ErrorState::ErrorState() :
mErrorCode( kNoError )
{
}


void ErrorState::setErrorCode( int errorCode )
{
    mErrorCode = errorCode;
}


void ErrorState::onEntry()
{
    // Stop driving
    Motors::stop();

    Beep::errorChime();

    Display::clear();
    Display::displayTopRow( "ERROR" );
    Display::setCursor( 1, 0 );
    Display::print( mErrorCode );
    return;
}


void ErrorState::onExit()
{
    // Never delete this event; just clear the error if we ever exit
    mErrorCode = kNoError;
}


bool ErrorState::onEvent( uint8_t event, int16_t param )
{
    return false;       // Abort any further event processing
}


