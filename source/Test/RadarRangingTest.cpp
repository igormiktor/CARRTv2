/*
    RadarRangingTest.cpp - Test of how the radar ranges.

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

#include "Drivers/Radar.h"

#include "NavigationMap.h"



void respondToInput();
void doPing();
void doPingSizeChange( char* token );
void doUpdateRadarPingSize( int pingSize );
void doScanIncrement( char* token );
void doMapRescale( char* token );
void doUpdateScale( int global, int local );
void doMapScan();
void doSlew( char* token );
void doInstructions();





Serial0 laptop;

int gGlobalCmPerGrid;
int gLocalCmPerGrid;
int gRadarPingSize;
int gScanIncrement;




int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    gGlobalCmPerGrid = 40;
    gLocalCmPerGrid = 10;
    gRadarPingSize = 3;
    gScanIncrement = 2;

    laptop.start( 115200 );

    Radar::init();
    Radar::slew( 0 );

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
    laptop.println( "Radar ranging test..." );
    laptop.println( "Enter a (or A) followed by relative azimuth to slew the radar" );
    laptop.println( "Enter p (or P) to ping the radar" );
    laptop.println( "Enter s (or S) to conduct a scan" );
    laptop.println( "Enter i (or I) followed by the scan increment (in deg)" );
    laptop.println( "Enter m (or M) followed by global scale and local scale to reset the Navigation map" );
    laptop.println( "Enter z (or Z) followed by nbr of ping cycles to reset the ping size" );
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



void doScanIncrement( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int scanIncrement = atoi( token );

        if ( scanIncrement > 0 && scanIncrement < 20 )
        {
            gScanIncrement = scanIncrement;
            laptop.print( "Scan increment (deg):  " );
            laptop.println( gScanIncrement );
        }
    }
}



void doPingSizeChange( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int pingSize = atoi( token );

        if ( pingSize > 0 && pingSize < 32 )
        {
            doUpdateRadarPingSize( pingSize );
        }
    }
}



void doUpdateRadarPingSize( int pingSize )
{
    gRadarPingSize = pingSize;

    laptop.print( "Radar scan ping size:  " );
    laptop.println( gRadarPingSize );
}



void doMapRescale( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int globalScale = atoi( token );

        token = strtok( 0, " \t" );
        if (  token )
        {
            int localScale = atoi( token );

            if ( globalScale > 0 && localScale > 0 )
            {
                doUpdateScale( globalScale, localScale );
            }
        }
    }
}



void doUpdateScale( int global, int local )
{
    gGlobalCmPerGrid = global;
    gLocalCmPerGrid = local;

    laptop.print( "Global cm/grid:  " );
    laptop.print( gGlobalCmPerGrid );
    laptop.print( "     Local cm/grid:  " );
    laptop.println( gLocalCmPerGrid );
}



void doMapScan()
{
    NavigationMap::init( gGlobalCmPerGrid, gLocalCmPerGrid );

    laptop.println( "Radar mapping scan..." );
    laptop.println( "Angle,      Distance,      X,      Y" );

    const float deg2rad = M_PI/180.0;

    for ( int slewAngle = -80; slewAngle < 81; slewAngle += gScanIncrement )
    {
        Radar::slew( slewAngle );
        delayMilliseconds( 500 );

        // Get a measurement and slew to next position
        int d = Radar::getDistanceInCm( gRadarPingSize );

        laptop.print( slewAngle );
        laptop.print( ",       " );
        laptop.print( d );

        if ( d != Radar::kNoRadarEcho )
        {
            // Record this observation
            float rad = deg2rad * slewAngle;
            float x = static_cast<float>( d ) * cos( rad );
            float y = static_cast<float>( d ) * sin( rad );

            laptop.print( ",         " );
            laptop.print( static_cast<int>( x + 0.5 ) );
            laptop.print( ",         " );
            laptop.print( static_cast<int>( y + 0.5 ) );

            NavigationMap::markObstacle( x + 0.5, y + 0.5 );
        }

        laptop.println();
    }

    Radar::slew( 0 );

    // Output the results

    char* globalMapOut = NavigationMap::getGlobalMap().dumpToStr();
    laptop.println( globalMapOut );
    free( globalMapOut );

    char* localMapOut = NavigationMap::getLocalMap().dumpToStr();
    laptop.println( localMapOut );
    free( localMapOut );

    laptop.println( "\n**** End Map ****\n" );
}



void doSlew( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int bearing = atoi( token );

        if ( -80 <= bearing && bearing <= 80 )
        {
            Radar::slew( bearing );
            laptop.print( "Radar slewed to azimuth:  " );
            laptop.println( bearing );

            doPing();
        }
    }
}



void respondToInput()
{
    char input[80];

    laptop.readLine( input, 80 );

    char* token;
    token = strtok( input, " \t" );

    if ( token )
    {
        switch ( *token )
        {
            case 'a':
            case 'A':
                // Slew command
                doSlew( token );
                break;

            case 'p':
            case 'P':
                doPing();
                break;

            case 's':
            case 'S':
                doMapScan();
                break;

            case 'i':
            case 'I':
                doScanIncrement( token );
                break;

            case 'm':
            case 'M':
                doMapRescale( token );
                break;

            case 'z':
            case 'Z':
                doPingSizeChange( token );
                break;
        }
    }
}

