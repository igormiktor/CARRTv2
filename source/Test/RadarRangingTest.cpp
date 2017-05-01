/*
    RadarRangingTest.cpp - Test of how the radar ranges.

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
#include <stdlib.h>
#include <string.h>

#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Radar.h"



void respondToInput();
void doPing();


Serial0 laptop;


int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    laptop.start( 115200 );

    Radar::init();
    Radar::slew( 0 );

    delayMilliseconds( 3000 );

    laptop.println( "Radar ranging test..." );

    laptop.println( "Enter a (or A) followed by relative azimuth to slew the radar" );

    laptop.println( "Enter p (or P) to ping the radar" );

    while ( 1 )
    {

        if ( laptop.available() )
        {
            respondToInput();
        }
        else
        {
            delayMilliseconds( 250 );
        }
    }
}


void doPing()
{
    int rng3 = Radar::getDistanceInCm( 3 );
    delayMilliseconds( 100 );
    int rng5 = Radar::getDistanceInCm( 5 );

    laptop.print( "Range:  " );
    laptop.print( rng3 );
    laptop.print( " (3 samples)    " );
    laptop.print( rng5 );
    laptop.println( " (5 samples)" );
}



void respondToInput()
{
    char input[80];

    laptop.readLine( input, 80 );
    if ( strchr( input, 'a' ) || strchr( input, 'A' ) )
    {
        // Skip the 'A'
        int offset = strcspn( input, "0123456789-+" );

        if ( offset != 0 )
        {
            int bearing = atoi( input + offset );

            if ( -80 <= bearing && bearing <= 80 )
            {
                Radar::slew( bearing );
                laptop.print( "Radar slewed to:  " );
                laptop.println( bearing );

                doPing();
            }
        }
    }

    if ( strchr( input, 'p' ) || strchr( input, 'P' ) )
    {
        doPing();
    }
}

