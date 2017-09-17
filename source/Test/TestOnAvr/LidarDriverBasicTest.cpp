/*
    LidarTest.cpp - Test of how the lidar ranges across various configurations.

    Copyright (c) 2017 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; withlaptop even the implied warranty of
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

#include "Drivers/Lidar.h"




void respondToInput();
void doPing();
void doUpdateLidarMode( int pingSize );
void doInstructions();





Serial0 laptop;

Lidar::Configuration gLidarMode;




int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    gLidarMode = Lidar::kDefault;

    laptop.start( 115200 );

    Lidar::init();

    delayMilliseconds( 3000 );

    doInstructions();

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



void doInstructions()
{
    laptop.println( "Lidar basic ranging test..." );
    laptop.println( "Enter p (or P) to ping the Lidar" );
    laptop.println( "Enter c (or C) followed by nbr set a lidar configuration" );
}



void doPing()
{
    int rng = Lidar::getDistanceInCm( 3 );

    laptop.print( "Range:  " );
    laptop.print( rng );
    laptop.println();
}



void doLidarModeChange( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int mode = atoi( token );

        if ( mode >= Lidar::kDefault && mode <= Lidar::kLowSensitivityButLowerError )
        {
            doUpdateLidarMode( mode );
        }
    }
}



void doUpdateLidarMode( int mode )
{
    static const char* modeStr[] =
    {
        "Default",
        "Short range, fast",
        "Shorter range, fastest",
        "Default range, faster at short range",
        "Maximum range",
        "High sensitivity, high error",
        "Low sensitivity, low error"
    };


    gLidarMode = static_cast<Lidar::Configuration>( mode );

    int err = Lidar::setConfiguration( gLidarMode );

    if ( err )
    {
        laptop.print( "Lidar error " );
        laptop.print( err );
        laptop.println( " when setting configuration" );
    }
    else
    {
        laptop.print( "Lidar mode:  " );
        laptop.println( modeStr[mode] );
    }
}



void respondToInput()
{
    char input[81];

    laptop.readLine( input, 80 );

    char* token;
    token = strtok( input, " \t" );

    if ( token )
    {
        switch ( *token )
        {
            case 'p':
            case 'P':
                doPing();
                break;

            case 'c':
            case 'C':
                doLidarModeChange( token );
                break;
        }
    }
}

