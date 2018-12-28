/*
    ErrorUnrecoverable.cpp - Handle unrecoverable errors for CARRT

    Copyright (c) 2017 Igor Mikolic-Torreira.  All right reserved.

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





#include "ErrorUnrecoverable.h"

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "Drivers/Beep.h"
#include "Drivers/Display.h"

#include "AVRTools/SystemClock.h"
#include "AVRTools/SimpleDelays.h"

#include "Utils/DebuggingMacros.h"





void handleUnrecoverableError( int errCode )
{
    Beep::errorChime();

    // If debugging output is on, display on debugging output
    DEBUG_PRINT( "ABORTING: Error code " );
    DEBUG_PRINTLN( errCode );

#ifndef CARRT_SUPPRESS_DISPLAY_UNRECOVERABLE_ERRORS
    Display::clear();               //1234567890123456
    Display::displayTopRowP16( PSTR( "! Err = " ) );
    Display::setCursor( 0, 9 );
    Display::print( errCode );
    Display::displayBottomRowP16( PSTR( "Aborting..." ) );
#endif

    // Put CARRT into an infinite delay loop
    while ( 1 )
    {
        // Two sec delays between beeps
        // Use simple delay function because system clock might not be reliable
        delayTenthsOfSeconds( 20 );
        Beep::errorChime();
    }
}



