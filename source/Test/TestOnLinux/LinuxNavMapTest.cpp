/*
    LinuxNavMapTest.cpp - Testing harness for the Navigation Map

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



#include <iostream>
#include <string.h>
#include <stdlib.h>


#include "NavigationMap.h"



void loadMap();
void displayMap();
void displayMap( const Map& m );
void goDown();
void goUp();
void goLeft();
void goRight();
void goUpLeft();
void goUpRight();

int main()
{
    NavigationMap::init();
    displayMap();
    loadMap();
    displayMap();

#if 0
    goDown();

    NavigationMap::init();
    loadMap();
    displayMap();
    goUp();

    NavigationMap::init();
    loadMap();
    displayMap();
    goLeft();

    NavigationMap::init();
    loadMap();
    displayMap();
    goRight();

    NavigationMap::init();
    loadMap();
    displayMap();
    goUpLeft();

    NavigationMap::init();
    loadMap();
    displayMap();
    goUpRight();
#endif

    std::cout << std::endl << "Done" << std::endl;
}




#if 0

void goUpLeft()
{
    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 8 , 8 )" << std::endl;

        NavigationMap::recenterMap( 8, 8 );

        displayMap();
    }

    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( -8, -8 )" << std::endl;

        NavigationMap::recenterMap( -8, -8 );

        displayMap();
    }
}


void goUpRight()
{
    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( -8 , 8 )" << std::endl;

        NavigationMap::recenterMap( -8, 8 );

        displayMap();
    }

    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 8, -8 )" << std::endl;

        NavigationMap::recenterMap( 8, -8 );

        displayMap();
    }
}


void goLeft()
{
    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 8 , 0 )" << std::endl;

        NavigationMap::recenterMap( 8, 0 );

        displayMap();
    }

    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( -8, 0 )" << std::endl;

        NavigationMap::recenterMap( -8, 0 );

        displayMap();
    }
}


void goRight()
{
    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( -8 , 0 )" << std::endl;

        NavigationMap::recenterMap( -8, 0 );

        displayMap();
    }

    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 8, 0 )" << std::endl;

        NavigationMap::recenterMap( 8, 0 );

        displayMap();
    }
}


void goDown()
{
    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 0 , 8 )" << std::endl;

        NavigationMap::recenterMap( 0, 8 );

        displayMap();
    }

    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 0 , -8 )" << std::endl;

        NavigationMap::recenterMap( 0, -8 );

        displayMap();
    }
}


void goUp()
{
    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 0 , -8 )" << std::endl;

        NavigationMap::recenterMap( 0, -8 );

        displayMap();
    }

    for ( int i = 0; i < 5; ++i )
    {
        std::cout << "Recenter map by ( 0 , 8 )" << std::endl;

        NavigationMap::recenterMap( 0, 8 );

        displayMap();
    }
}


#endif



void loadMap()
{
    NavigationMap::markObstacle( 0, 0 );

    for ( int i = 100; i < 1500; i += 200 )
    {
        NavigationMap::markObstacle( -i, 0 );
        NavigationMap::markObstacle( i, 0 );
        NavigationMap::markObstacle( 0, -i );
        NavigationMap::markObstacle( 0, i );
        NavigationMap::markObstacle( -i, -i );
        NavigationMap::markObstacle( i, -i );
        NavigationMap::markObstacle( -i, i );
        NavigationMap::markObstacle( i, i );
    }
}








void displayMap()
{
#if CARRT_DEBUG_NAVIGATION_MAP
    displayInternalMaps();
#endif

    std::cout << std::endl << "Global map:" << std::endl;
    displayMap( NavigationMap::getGlobalMap() );

    std::cout << std::endl << "Local map:" << std::endl;
    displayMap( NavigationMap::getLocalMap() );
}




#if CARRT_DEBUG_NAVIGATION_MAP

void displayInternalMaps()
{
    // Display the raw internal map (global)
    std::cout << std::endl << "Global map (internal grid):" << std::endl;
    std::cout << NavigationMap::getGlobalMap().minXCoord() << ", " << NavigationMap::getGlobalMap().minYCoord()
        << " <-> " << NavigationMap::getGlobalMap().maxXCoord() << ", " << NavigationMap::getGlobalMap().maxYCoord() << std::endl;
    bool isGlobalObstacle;
    bool onGlobalMap = NavigationMap::getGlobalMap().isThereAnObstacle( 3200, 3200, &isGlobalObstacle );
    std::cout << "Off-grid ( 3200, 3200 ): " << ( onGlobalMap ? ( isGlobalObstacle ? '*' : '.' ) : '!' ) << std::endl;
    onGlobalMap = NavigationMap::getGlobalMap().isThereAnObstacle( 1200, 1200, &isGlobalObstacle );
    std::cout << "Off-grid ( 1200, 1200 ): " << ( onGlobalMap ? ( isGlobalObstacle ? '*' : '.' ) : '!' ) << std::endl;
    char* tmpG = NavigationMap::getGlobalMap().dumpToStr();
    std::cout << tmpG << std::endl;
    free( tmpG );

    // Display the raw internal map (global)
    std::cout << std::endl << "Local map (grid):" << std::endl;
    std::cout << NavigationMap::getLocalMap().minXCoord() << ", " << NavigationMap::getLocalMap().minYCoord()
        << " <-> " << NavigationMap::getLocalMap().maxXCoord() << ", " << NavigationMap::getLocalMap().maxYCoord() << std::endl;
    bool isLocalObstacle;
    bool onLocalMap = NavigationMap::getLocalMap().isThereAnObstacle( 800, 800, &isLocalObstacle );
    std::cout << "Off-grid ( 800, 800 ): " << ( onLocalMap ? ( isLocalObstacle ? '*' : '.' ) : '!' ) << std::endl;
    onLocalMap = NavigationMap::getLocalMap().isThereAnObstacle( 300, 300, &isLocalObstacle );
    std::cout << "Off-grid ( 300, 300 ): " << ( onLocalMap ? ( isLocalObstacle ? '*' : '.' ) : '!' ) << std::endl;
    char* tmpL = NavigationMap::getLocalMap().dumpToStr();
    std::cout << tmpL << std::endl;
    free( tmpL );
}

#endif  // CARRT_DEBUG_NAVIGATION_MAP


void displayMap( const Map& map )
{
    // Display the map

    std::cout << "Display the map..." << std::endl;

    int digit;
    std::cout << ' ';
    for ( int x = map.minXCoord(), digit = 1; x < map.maxXCoord(); x += map.cmPerGrid(), ++digit )
    {
        digit %= 10;
        std::cout << digit;
    }
    std::cout << std::endl;

    for ( int y = map.minYCoord(), digit = 1; y < map.maxYCoord(); y += map.cmPerGrid(), ++digit )
    {
        digit %= 10;
        std::cout << digit;

        for ( int x = map.minXCoord(), digit = 1; x < map.maxXCoord(); x += map.cmPerGrid() )
        {
            bool isObstacle;
            bool onMap = NavigationMap::isThereAnObstacle( x, y, &isObstacle );
            if ( !onMap )
            {
                std::cout << '!';
            }
            else
            {
                if ( isObstacle )
                {
                    std::cout << '*';
                }
                else
                {
                    std::cout << '.';
                }
            }
        }
        std::cout << std::endl;
    }
}




