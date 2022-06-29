/*
    L3GD20Test.cpp - Testing harness for the L3GD20 Gyroscope driver

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
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/L3GD20.h"

#include "Utils/VectorUtils.h"


Vector3Long testBlockRead();


int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Serial0 out;
    out.start( 115200 );

    L3GD20::init();

    delayMilliseconds( 1000 );

    out.println( "L3GD20 Gyroscope test..." );

    out.println( "Calculating at rest -- do not move CARRT" );

    const int nZero = 5;
    Vector3Float rate0( 0.0, 0.0, 0.0 );
    for ( int i = 0; i < nZero; ++i )
    {
        rate0 += L3GD20::getAngularRatesDegreesPerSecond();
        out.print( '.' );
        delayMilliseconds( 1000 );
    }
    rate0 /= static_cast<float>( nZero );
    out.println();
    out.print( "At rest:  dx/dt = " );
    out.print( rate0.x );
    out.print( "  dy/dt = " );
    out.print( rate0.y );
    out.print( "  dz/dt = " );
    out.println( rate0.z );
    out.println();
    out.println();
    out.println( "Begin test...\n" );

    while ( 1 )
    {
        Vector3Float rate = L3GD20::getAngularRatesDegreesPerSecond() - rate0;

        out.print( "dx/dt = " );
        out.print( rate.x );
        out.print( "  dy/dt = " );
        out.print( rate.y );
        out.print( "  dz/dt = " );
        out.println( rate.z );
        out.println();

        for ( int i = 0; i < 5; ++i )
        {
            out.print( i + 1 );
            out.println( "..." );
            delayMilliseconds( 1000 );
        }
        out.println();

        // Vector3Long rateAvg = testBlockRead();


    }
}



