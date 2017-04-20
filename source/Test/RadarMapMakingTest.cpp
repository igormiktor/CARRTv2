/*
    RadarMapMakingTest.cpp - Test building a map using the radar.

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

#include <math.h>

#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Radar.h"

#include "NavigationMap.h"



int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Serial0 out;
    out.start( 115200 );

    Radar::init();
    Radar::slew( 0 );

    NavigationMap::init();

    delayMilliseconds( 1000 );

    out.println( "Radar mapping test..." );

    int d = Radar::getDistanceInCm();

    int slewAngle = -80;
    Radar::slew( slewAngle );
    delayMilliseconds( 250 );

    const float deg2rad = M_PI/180.0;

    for ( ; slewAngle < 85; slewAngle += 10 )
    {
        // Record this observation
        float rad = deg2rad * slewAngle;
        float x = d * cos( rad );
        float y = d * sin( rad );

        NavigationMap::markObstacle( x + 0.5, y + 0.5 );

        // Get a measurement and slew to next position
        d = Radar::getDistanceInCm();
        Radar::slew( slewAngle );
    }

    while ( 1 )
    {
    }
}

