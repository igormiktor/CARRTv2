/*
    BeepTest.cpp - Testing harness for the audio driver 

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




#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/USART0.h"

#include "Drivers/Beep.h"


int main()
{
    initSystem();
    initSystemClock();
    
    Serial0 out;
    out.start( 115200 );
    
    Beep::initBeep();
    
    out.println( "Beep Test" );

    while ( 1 )
    {
        delayMilliseconds( 1000 );

        Beep::alert();

        delayMilliseconds( 1000 );
        
        Beep::chirp();

        delayMilliseconds( 1000 );

        Beep::beep();
        
        delayMilliseconds( 1000 );

        Beep::errorChime();
        
        delayMilliseconds( 1000 );

        Beep::beepOn();
        
        delayMilliseconds( 1000 );

        Beep::beepOff();
        
        out.println( "End of test" );
    }
}


