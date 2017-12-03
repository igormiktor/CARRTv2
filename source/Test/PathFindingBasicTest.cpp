/*
    PathFindingTest.cpp - Test of lidar map-making and then
    finding a path to a goal.

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

#include "Drivers/Beep.h"
#include "Drivers/Lidar.h"

#include "NavigationMap.h"

#include "PathSearch/PathFinder.h"
#include "PathSearch/PathFinderMap.h"
#include "PathSearch/FrontierList.h"
#include "PathSearch/ExploredList.h"

#include "Utils/DebuggingMacros.h"




void makeMap();
void loadMap();
void findPath( int origX, int origY, int destX, int destY );




Lidar::Configuration gLidarMode;
int gGlobalCmPerGrid;
int gLocalCmPerGrid;
int gScanIncrement;




class DisplayMap
{
public:
    DisplayMap( PathFinder::Path* p, int startX, int startY, int goalX, int goalY, const Map* map );
    DisplayMap( const DisplayMap& dm );
    ~DisplayMap();

    void display();

private:

    const Map* mMap;
    char* data;
};







int main()
{
    initSystem();
    initSystemClock();
    Beep::initBeep();

    I2cMaster::start();

    gLidarMode = Lidar::kDefault;
    gGlobalCmPerGrid = 32;
    gLocalCmPerGrid = 16;

    initDebugSerial();

    Lidar::init();

    delayMilliseconds( 1000 );

    makeMap();

#if CARRT_PFBT_USE_MAP1
    int originX = 0;
    int originY = 0;
    int goalX = 300;
    int goalY = 200;
#elif CARRT_PFBT_USE_MAP2
    int originX = 0;
    int originY = 0;
    int goalX = 300;
    int goalY = 300;
#endif

    findPath( originX, originY, goalX, goalY );


    gDebugSerial.println( "\n\nFinished\n" );

    stopDebugSerial();
}



void makeMap()
{
    NavigationMap::init( gGlobalCmPerGrid, gLocalCmPerGrid );

    loadMap();

    gDebugSerial.println( "\n\nBegin Map\n" );

    char* globalMapOut = NavigationMap::getGlobalMap().dumpToStr();

    gDebugSerial.println( globalMapOut );

    free( globalMapOut );

    char* localMapOut = NavigationMap::getLocalMap().dumpToStr();

    gDebugSerial.println( localMapOut );

    free( localMapOut );

    gDebugSerial.println( "\n\nEnd Map\n" );
}



#if CARRT_PFBT_USE_MAP1

void loadMap()
{
     // Build top wall
    for ( int i = -200; i < 201; ++i )
    {
        NavigationMap::markObstacle( 140, i );
    }

    // Build side walls
    for ( int i = -800; i < 141; ++i )
    {
        NavigationMap::markObstacle( i, -200 );
        NavigationMap::markObstacle( i, +200 );
    }

    // Add a window
    for ( int i = -220; i < -150; ++i )
    {
        NavigationMap::markClear( i, -200 );
    }
}

#elif CARRT_PFBT_USE_MAP2

void loadMap()
{
    NavigationMap::markObstacle( 67, -380 );
    NavigationMap::markObstacle( 66, -311 );
    NavigationMap::markObstacle( 61, -243 );
    NavigationMap::markObstacle( 65, -226 );
    NavigationMap::markObstacle( 66, -202 );
    NavigationMap::markObstacle( 117, -289 );
    NavigationMap::markObstacle( 115, -257 );
    NavigationMap::markObstacle( 112, -229 );
    NavigationMap::markObstacle( 36, -67 );
    NavigationMap::markObstacle( 32, -55 );
    NavigationMap::markObstacle( 34, -53 );
    NavigationMap::markObstacle( 35, -50 );
    NavigationMap::markObstacle( 35, -48 );
    NavigationMap::markObstacle( 41, -51 );
    NavigationMap::markObstacle( 44, -52 );
    NavigationMap::markObstacle( 48, -52 );
    NavigationMap::markObstacle( 51, -52 );
    NavigationMap::markObstacle( 55, -52 );
    NavigationMap::markObstacle( 60, -53 );
    NavigationMap::markObstacle( 65, -54 );
    NavigationMap::markObstacle( 71, -54 );
    NavigationMap::markObstacle( 78, -55 );
    NavigationMap::markObstacle( 85, -57 );
    NavigationMap::markObstacle( 92, -57 );
    NavigationMap::markObstacle( 101, -58 );
    NavigationMap::markObstacle( 113, -59 );
    NavigationMap::markObstacle( 121, -58 );
    NavigationMap::markObstacle( 125, -55 );
    NavigationMap::markObstacle( 125, -50 );
    NavigationMap::markObstacle( 124, -44 );
    NavigationMap::markObstacle( 123, -39 );
    NavigationMap::markObstacle( 136, -38 );
    NavigationMap::markObstacle( 232, -57 );
    NavigationMap::markObstacle( 231, -48 );
    NavigationMap::markObstacle( 231, -40 );
    NavigationMap::markObstacle( 233, -32 );
    NavigationMap::markObstacle( 233, -23 );
    NavigationMap::markObstacle( 232, -15 );
    NavigationMap::markObstacle( 230, -7 );
    NavigationMap::markObstacle( 231, 0 );
    NavigationMap::markObstacle( 231, 8 );
    NavigationMap::markObstacle( 230, 16 );
    NavigationMap::markObstacle( 232, 24 );
    NavigationMap::markObstacle( 232, 33 );
    NavigationMap::markObstacle( 231, 41 );
    NavigationMap::markObstacle( 233, 49 );
    NavigationMap::markObstacle( 232, 58 );
    NavigationMap::markObstacle( 124, 36 );
    NavigationMap::markObstacle( 124, 40 );
    NavigationMap::markObstacle( 128, 47 );
    NavigationMap::markObstacle( 127, 51 );
    NavigationMap::markObstacle( 127, 57 );
    NavigationMap::markObstacle( 116, 57 );
    NavigationMap::markObstacle( 106, 56 );
    NavigationMap::markObstacle( 95, 55 );
    NavigationMap::markObstacle( 87, 55 );
    NavigationMap::markObstacle( 80, 54 );
    NavigationMap::markObstacle( 74, 53 );
    NavigationMap::markObstacle( 65, 51 );
    NavigationMap::markObstacle( 61, 51 );
    NavigationMap::markObstacle( 62, 56 );
    NavigationMap::markObstacle( 62, 60 );
    NavigationMap::markObstacle( 74, 76 );
    NavigationMap::markObstacle( 76, 85 );
    NavigationMap::markObstacle( 77, 92 );
    NavigationMap::markObstacle( 78, 99 );
    NavigationMap::markObstacle( 78, 108 );
    NavigationMap::markObstacle( 78, 115 );
    NavigationMap::markObstacle( 78, 125 );
    NavigationMap::markObstacle( 66, 115 );
    NavigationMap::markObstacle( 64, 120 );
    NavigationMap::markObstacle( 58, 120 );
    NavigationMap::markObstacle( 53, 119 );
    NavigationMap::markObstacle( 46, 115 );
    NavigationMap::markObstacle( 43, 119 );
    NavigationMap::markObstacle( 40, 122 );
    NavigationMap::markObstacle( 40, 140 );
    NavigationMap::markObstacle( 53, 214 );
    NavigationMap::markObstacle( 62, 291 );
    NavigationMap::markObstacle( 63, 357 );
}

#endif


void findPath( int origX, int origY, int destX, int destY )
{
    // Now do the finding part
    PathFinder::Path* p = PathFinder::findPath( origX, origY, destX, destY, NavigationMap::getGlobalMap() );

    DisplayMap dm( p, 0, 0, destX, destY, NavigationMap::getGlobalMapPtr() );
    dm.display();

    delete p;
}




DisplayMap::DisplayMap( PathFinder::Path* p, int startX, int startY, int goalX, int goalY, const Map* map )
: mMap( map )
{
    int minX = mMap->minXCoord();
    int minY = mMap->minYCoord();

    int incr = mMap->cmPerGrid();

    int gridSizeX = mMap->sizeGridX();
    int gridSizeY = mMap->sizeGridY();

    // First load the map
    data = static_cast<char*>( malloc( gridSizeX * gridSizeY ) );

    for ( int y = 0; y < gridSizeY; ++y )
    {
        for ( int x = 0; x < gridSizeX; ++x )
        {
            int index = x  + y*gridSizeX;

            int xx = x*incr + minX;
            int yy = y*incr + minY;
            bool isObstacle;
            bool onMap = mMap->isThereAnObstacle( xx, yy, &isObstacle );
            char c = '.';
            if ( !onMap )
            {
                c = '!';
            }
            else if ( isObstacle )
            {
                c = 'X';
            }
            data[index] = c;
        }
    }


    // Now overlay the path
    if ( p )
    {
        gDebugSerial.println( "Path list" );

        PathFinder::WayPoint* wp = p->pop();
        int n = 1;
        while ( wp )
        {
            gDebugSerial.print( n );
            gDebugSerial.print( " (" );
            gDebugSerial.print( wp->x() );
            gDebugSerial.print( ", " );
            gDebugSerial.print( wp->y() );
            gDebugSerial.println( ')' );

            int x = ( wp->x() - minX ) / incr;
            int y = ( wp->y() - minY ) / incr;

            int index = x + y*gridSizeX;
            data[index] = '0' + (n % 10);
            ++n;

            wp = p->pop();
        }
        gDebugSerial.println( "End path list" );
    }

    // Now overlay the start and goal

    int index = ( (startX - minX) / incr ) + ( (startY - minY) / incr ) * gridSizeX;
    data[index] = 'S';

    index = ( (goalX - minX) / incr ) + ( (goalY - minY) / incr ) * gridSizeX;
    data[index] = 'G';
}



DisplayMap::~DisplayMap()
{
    free( data );
}



void DisplayMap::display()
{
    // Display the map

    int maxX = mMap->sizeGridX();
    int maxY = mMap->sizeGridY();

    gDebugSerial.println();
    gDebugSerial.println( "Display map..." );

//    int digit;
    gDebugSerial.print( ' ' );
    for ( int y = 0, digit = 1; y < maxY; ++y, ++digit )
    {
        digit %= 10;
        gDebugSerial.print( digit );
    }
    gDebugSerial.println();

    for ( int y = 0, digit = 1; y < maxY; ++y, ++digit )
    {
        digit %= 10;
        gDebugSerial.print( digit );

        for ( int x = 0; x < maxX; ++x )
        {
            int index = x + y * maxY;

            gDebugSerial.print( data[index] );
        }
        gDebugSerial.println();
    }
    gDebugSerial.println();
}





