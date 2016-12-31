/*
    RadarTimingTest.cpp - Test how quickly the radar works.

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
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Radar.h"





int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Serial0 out;
    out.start( 115200 );

    Radar::init();
    Radar::slew( 0 );

    delayMilliseconds( 1000 );

    out.println( "Radar timing test..." );

    const int kN = 200;

    // Compute overhead
    unsigned long overhead = 0;
    for ( int n = 0; n < kN; ++n )
    {
        unsigned long t0 = millis();
        overhead += millis() - t0;
    }
    overhead /= kN;

    out.print( "Overhead is:  " );
    out.print( overhead / kN );
    out.println( " ms" );

    // Time the radar
    unsigned long timing = 0;
    for ( int n = 0; n < kN; ++n )
    {
        unsigned long t0 = millis();
        int d = Radar::getDistanceInCm();
        timing += millis() - t0;
    }
    timing -= overhead;
    timing /= kN;

    out.print( "Timing of radar is:  " );
    out.print( timing );
    out.println( " ms" );



    while ( 1 )
    {
    }
}

