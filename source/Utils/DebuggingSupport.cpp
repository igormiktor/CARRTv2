/*
    DebuggingSupport.cpp - Simply includes (or not) AVRTools/USART0.cpp
    into the buid dependent on whether serial debugging is enabled.

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




#if CARRT_ENABLE_DEBUG_SERIAL


#include <avr/pgmspace.h>

#include "AVRTools/SystemClock.h"

// If debugging is enabled include all the USART0 code

#include "AVRTools/USART0.h"

#include "Drivers/Beep.h"


// Declare the global serial object for debugging output
Serial0 gDebugSerial;




// Function to initialize the debug serial connection
void initDebugSerial()
{
    gDebugSerial.start( 115200 );

    // Give serial a chance to stabilize
    delayMilliseconds( 500 );

    char tmp[32];
    strcpy_P( tmp, PSTR( "CARRT Debugging Output...\n" ) );
    gDebugSerial.println( tmp );
}



// Function to close the debug serial connection
void stopDebugSerial()
{
    char tmp[36];
    strcpy_P( tmp, PSTR( "CARRT Debugging Output Stopped\n" ) );
    gDebugSerial.println( tmp );

    // Give serial a chance to stabilize
    delayMilliseconds( 500 );

    gDebugSerial.stop();
}



// Function to print a string in program space
void printDebugSerialPStr( PGM_P str, bool addLn )
{
    char tmp[121];
    strncpy_P( tmp, str, 120 );
    // Ensure null-terminated (if string is too long)
    tmp[120] = 0;

    gDebugSerial.print( tmp, addLn );
}



// Function to make a special debug beep
void doDebugBeep()
{
    for ( uint8_t i = 0; i < 3; ++i )
    {
        Beep::chirp();
        delayMilliseconds( Beep::kBeepDefaultBeepTone );
    }
}



#endif
