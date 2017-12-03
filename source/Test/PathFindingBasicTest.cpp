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

    findPath( 0, 0, 300, 200 );


    gDebugSerial.println( "\n\nFinished\n" );

    stopDebugSerial();
}



void makeMap()
{
    NavigationMap::init( gGlobalCmPerGrid, gLocalCmPerGrid );

     // Build a wall from ( 100, -200 ) to ( 100, 200 )
    for ( int i = -200; i < 201; ++i )
    {
        NavigationMap::markObstacle( 140, i );
    }

    // Build walls from ( 100, +/-180 ) to ( -300, +/-180 )
    for ( int i = -800; i < 101; ++i )
    {
        NavigationMap::markObstacle( i, -200 );
        NavigationMap::markObstacle( i, +200 );
    }

    // Add a window
    for ( int i = -200; i < -150; ++i )
    {
        NavigationMap::markClear( i, -200 );
    }

    gDebugSerial.println( "\n\nBegin Map\n" );

    char* globalMapOut = NavigationMap::getGlobalMap().dumpToStr();

    gDebugSerial.println( globalMapOut );

    free( globalMapOut );

    char* localMapOut = NavigationMap::getLocalMap().dumpToStr();

    gDebugSerial.println( localMapOut );

    free( localMapOut );

    gDebugSerial.println( "\n\nEnd Map\n" );
}



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
/*
        gDebugSerial.print( n );
        gDebugSerial.print( " (" );
        gDebugSerial.print( goalX );
        gDebugSerial.print( ", " );
        gDebugSerial.print( goalY );
        gDebugSerial.println( ')' );
*/
        gDebugSerial.println( "End path list" );
    }

    // Now overlay the start and goal

    int index = ( (startX - minX) + (startY - minY) * gridSizeX ) / incr;
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





