/*
    DebuggingSupport.cpp - Simply includes (or not) AVRTools/USART0.cpp
    into the buid dependent on whether serial debugging is enabled.

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




#if CARRT_ENABLE_DEBUG_SERIAL


// If debugging is enabled include all the USART0 code

#include "AVRTools/USART0.cpp"



// Declare the global serial object for debugging output
Serial0 gDebugSerial;



#include <avr/pgmspace.h>

// Function to initialize the debug serial connection
void initDebugSerial()
{
    gDebugSerial.start( 115200 );
    char tmp[30];
    strcpy_P( tmp, PSTR( "CARRT Debug Output...\n" ) );
    gDebugSerial.println( tmp );
}


#endif
