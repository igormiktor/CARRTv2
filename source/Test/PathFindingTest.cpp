/*
    PathFindingTest.cpp - Test of lidar map-making and then
    finding a path to a goal.

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
#include <string.h>

#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Beep.h"
#include "Drivers/Lidar.h"

#include "NavigationMap.h"

#include "PathSearch/PathFinder.h"
#include "PathSearch/PathFinderMap.h"
#include "PathSearch/FrontierList.h"
#include "PathSearch/ExploredList.h"

#include "DisplayMap.h"

#include "Utils/DebuggingMacros.h"



void respondToInput();
void doScanIncrement( char* token );
void doMapRescale( char* token );
void doUpdateScale( int global, int local );
void doInstructions();
void doFindPath( char* token );
void doMakeMapAndFindPath( int destX, int destY );
void executeMapScan();




Lidar::Configuration gLidarMode;
int gGlobalCmPerGrid;
int gLocalCmPerGrid;
int gScanIncrement;







int main()
{
    initSystem();
    initSystemClock();
    Beep::initBeep();

    I2cMaster::start();

    gLidarMode = Lidar::kDefault;
    gGlobalCmPerGrid = 32;
    gLocalCmPerGrid = 16;
    gScanIncrement = 2;

    initDebugSerial();

    Lidar::init();

    delayMilliseconds( 1000 );

    doInstructions();

    while ( 1 )
    {
        if ( gDebugSerial.available() )
        {
            respondToInput();
        }
        else
        {
            delayMilliseconds( 250 );
        }
    }

    stopDebugSerial();

}



void doInstructions()
{
    gDebugSerial.println( "Path finding test:" );
    gDebugSerial.println( "Enter i (or I) followed by the scan incr (deg)" );
    gDebugSerial.println( "Enter m (or M) followed by global scale and local scale (cm) to reset the Navigation map" );
    gDebugSerial.println( "Enter p (or P) followed by destn coords x, y (rel, cm) to build a map and find a path" );
}




void doScanIncrement( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int scanIncrement = atoi( token );

        if ( scanIncrement > 0 && scanIncrement < 21 )
        {
            gScanIncrement = scanIncrement;
            gDebugSerial.print( "Scan increment (deg):  " );
            gDebugSerial.println( gScanIncrement );
        }
    }
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

    gDebugSerial.print( "Global cm/grid:  " );
    gDebugSerial.print( gGlobalCmPerGrid );
    gDebugSerial.print( "     Local cm/grid:  " );
    gDebugSerial.println( gLocalCmPerGrid );
}



void executeMapScan()
{
    gDebugSerial.println( "Lidar mapping scan..." );
    gDebugSerial.println( "Angle,      Distance,      X,      Y" );

    const float deg2rad = M_PI/180.0;

    for ( int slewAngle = -80; slewAngle < 81; slewAngle += gScanIncrement )
    {
        Lidar::slew( slewAngle );
        delayMilliseconds( 500 );

        int d;
        int err = Lidar::getMedianDistanceInCm( &d );

        gDebugSerial.print( slewAngle );
        gDebugSerial.print( ",       " );

        if ( !err )
        {
            gDebugSerial.print( d );

            // Record this observation
            float rad = deg2rad * slewAngle;
            float x = static_cast<float>( d ) * cos( rad );
            float y = static_cast<float>( d ) * sin( rad );

            gDebugSerial.print( ",         " );
            gDebugSerial.print( static_cast<int>( x + 0.5 ) );
            gDebugSerial.print( ",         " );
            gDebugSerial.print( static_cast<int>( y + 0.5 ) );

            NavigationMap::markObstacle( x + 0.5, y + 0.5 );
        }
        else
        {
            gDebugSerial.print( ", , , " );
            gDebugSerial.print( err );
        }

        gDebugSerial.println();
    }
    gDebugSerial.println();

    Lidar::slew( 0 );

    // Output the results

    gDebugSerial.print( "Global cm/grid:  " );
    gDebugSerial.print( gGlobalCmPerGrid );
    gDebugSerial.print( "     Local cm/grid:  " );
    gDebugSerial.println( gLocalCmPerGrid );
    gDebugSerial.println();

    char* globalMapOut = NavigationMap::getGlobalMap().dumpToStr();
    gDebugSerial.println( globalMapOut );
    free( globalMapOut );

    char* localMapOut = NavigationMap::getLocalMap().dumpToStr();
    gDebugSerial.println( localMapOut );
    free( localMapOut );

    gDebugSerial.println( "\n**** End Map ****\n" );
}





void doFindPath( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int destX = atoi( token );

        token = strtok( 0, " \t" );
        if (  token )
        {
            int destY = atoi( token );

            doMakeMapAndFindPath( destX, destY );
        }
    }
}





void doMakeMapAndFindPath( int destX, int destY )
{
    NavigationMap::init( gGlobalCmPerGrid, gLocalCmPerGrid );

    executeMapScan();

    // Now do the finding part
    PathFinder::Path* p = PathFinder::findPath( 0, 0, destX, destY, NavigationMap::getGlobalMap() );

    DisplayMap dm( p, 0, 0, destX, destY, NavigationMap::getGlobalMapPtr() );
    dm.display();

    delete p;
}



void respondToInput()
{
    char input[81];

    gDebugSerial.readLine( input, 80 );

    char* token;
    token = strtok( input, " \t" );

    if ( token )
    {
        switch ( *token )
        {
            case 'i':
            case 'I':
                doScanIncrement( token );
                break;

            case 'm':
            case 'M':
                doMapRescale( token );
                break;

            case 'p':
            case 'P':
                doFindPath( token );
                break;
        }
    }
}





