/*
    SonarTimingTest.cpp - Test how quickly the radar works.

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


#include <limits.h>

#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Sonar.h"





int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Serial0 out;
    out.start( 115200 );

    Sonar::init();
    Sonar::slew( 0 );

    delayMilliseconds( 1000 );

    out.println( "Sonar timing test..." );

    int d = Sonar::getDistanceInCm();
    out.print( "Distance is " );
    out.print( d );
    out.println( " cm" );
#if 0
    d = Sonar::getQuickDistanceInCm();
    out.print( "Quick Distance is " );
    out.print( d );
    out.println( " cm" );
#endif
    d = Sonar::getSinglePingDistanceInCm();
    out.print( "Single Ping Distance is " );
    out.print( d );
    out.println( " cm" );


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

    {
        int max = 0;
        int min = INT_MAX;

        // Time the radar
        unsigned long timing = 0;
        for ( int n = 0; n < kN; ++n )
        {
            unsigned long t0 = millis();
            int d = Sonar::getDistanceInCm();
            timing += millis() - t0;

            if ( d < min )  min = d;
            if ( d > max )  max = d;
        }
        timing -= overhead;
        timing /= kN;

        out.print( "Timing of radar is:  " );
        out.print( timing );
        out.println( " ms" );
        out.print( "Spread from " );
        out.print( min );
        out.print( " to " );
        out.print( max );
        out.println( " cm" );
    }

#if 0
{
        int max = 0;
        int min = INT_MAX;

        // Time the quicker radar
        unsigned long timing = 0;
        for ( int n = 0; n < kN; ++n )
        {
            unsigned long t0 = millis();
            int d = Sonar::getQuickDistanceInCm();
            timing += millis() - t0;

            if ( d < min )  min = d;
            if ( d > max )  max = d;
        }
        timing -= overhead;
        timing /= kN;

        out.print( "Timing of quick radar is:  " );
        out.print( timing );
        out.println( " ms" );
        out.print( "Spread from " );
        out.print( min );
        out.print( " to " );
        out.print( max );
        out.println( " cm" );
    }
#endif

    {
        int max = 0;
        int min = INT_MAX;

        // Time the single ping radar
        unsigned long timing = 0;
        for ( int n = 0; n < kN; ++n )
        {
            unsigned long t0 = millis();
            int d = Sonar::getSinglePingDistanceInCm();
            timing += millis() - t0;

            if ( d < min )  min = d;
            if ( d > max )  max = d;
        }
        timing -= overhead;
        timing /= kN;

        out.print( "Timing of single ping radar is:  " );
        out.print( timing );
        out.println( " ms" );
        out.print( "Spread from " );
        out.print( min );
        out.print( " to " );
        out.print( max );
        out.println( " cm" );
    }

    while ( 1 )
    {
    }
}

