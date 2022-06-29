/*
    SonarMapMakingTest.cpp - Test building a map using the radar.

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

#include <math.h>
#include <stdlib.h>

#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Sonar.h"

#include "NavigationMap.h"



int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Serial0 out;
    out.start( 115200 );

    Sonar::init();
    Sonar::slew( 0 );

    NavigationMap::init( 40, 10 );

    delayMilliseconds( 3000 );

    out.println( "Sonar mapping test..." );

    out.println( "Angle,      Distance,      X,      Y" );

    const float deg2rad = M_PI/180.0;

    for ( int slewAngle = -80; slewAngle < 81; slewAngle += 1 )
    {
        Sonar::slew( slewAngle );
        delayMilliseconds( 500 );

        // Get a measurement and slew to next position
        int d = Sonar::getDistanceInCm( 3 );

        out.print( slewAngle );
        out.print( ",       " );
        out.print( d );

        if ( d != Sonar::kNoSonarEcho )
        {
            // Record this observation
            float rad = deg2rad * slewAngle;
            float x = static_cast<float>( d ) * cos( rad );
            float y = static_cast<float>( d ) * sin( rad );

            out.print( ",         " );
            out.print( static_cast<int>( x + 0.5 ) );
            out.print( ",         " );
            out.print( static_cast<int>( y + 0.5 ) );

            NavigationMap::markObstacle( x + 0.5, y + 0.5 );
        }

        out.println();
    }

    // Output the results

    char* globalMapOut = NavigationMap::getGlobalMap().dumpToStr();

    out.println( globalMapOut );

    free( globalMapOut );

    char* localMapOut = NavigationMap::getLocalMap().dumpToStr();

    out.println( localMapOut );

    free( localMapOut );

    out.println( "\n\n**** Done ****\n" );


    while ( 1 )
    {
    }
}

