/*
    NavigatorTimingTest.cpp - Test how nav updates take.

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



#include "AVRTools/InitSystem.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/USART0.h"

#include "Navigator.h"

#include "Drivers/LSM303DLHC.h"
#include "Drivers/L3GD20.h"






int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    LSM303DLHC::init();
    L3GD20::init();

    Serial0 out;
    out.start( 115200 );

    delayMilliseconds( 1000 );

    out.println( "Nav update timing test..." );

    out.println( "Navigator initializing..." );
    Navigator::init();

    out.println( "Start timing..." );

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

    while ( 1 )
    {
        // Time the navigator
        Navigator::movingStraight();
        unsigned long timing = 0;
        for ( int n = 0; n < kN; ++n )
        {
            unsigned long t0 = millis();
            Navigator::doNavUpdate();
            timing += millis() - t0;
        }
        timing -= overhead;
        float timingF = static_cast<float>( timing ) / kN;

        out.print( "Timing of a moving straight Nav Update is:  " );
        out.print( timingF );
        out.println( " ms" );

        // Time the navigator
        Navigator::movingTurning();
        timing = 0;
        for ( int n = 0; n < kN; ++n )
        {
            unsigned long t0 = millis();
            Navigator::doNavUpdate();
            timing += millis() - t0;
        }
        timing -= overhead;
        timingF = static_cast<float>( timing ) / kN;

        out.print( "Timing of a moving turning Nav Update is:  " );
        out.print( timingF );
        out.println( " ms" );

        out.println();
    }
}

