/*
    LinuxPathFinderTest.cpp - Test the path finding algorithms.

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
#include <math.h>

#include "NavigationMap.h"

#include "PathSearch/PathFinder.h"
#include "PathSearch/PathFinderMap.h"
#include "PathSearch/FrontierList.h"
#include "PathSearch/ExploredList.h"



#define MAP2    1



#define kGoalX          10
#define kGoalY          10


#define kStartX         -10
#define kStartY         -10

using namespace PathFinder;




void setUpNavMap();

void dumpPath( Path* p );

void displayRawMap();



class DisplayMap
{
public:
    DisplayMap( Path* p, int startX, int startY, int goalX, int goalY );
    ~DisplayMap();

    void display();

private:

    char* data;
};



DisplayMap::DisplayMap( Path* p, int startX, int startY, int goalX, int goalY )
{
    int maxX = NavigationMap::sizeX();
    int maxY = NavigationMap::sizeY();

    int offsetX = NavigationMap::minXCoord();
    int offsetY = NavigationMap::minYCoord();

    // First load the map
    data = static_cast<char*>( malloc( maxX * maxY ) );

    for ( int y = 0; y < maxY; ++y )
    {
        for ( int x = 0; x < maxX; ++x )
        {
            int index = x  + y*maxX;

//            std::cout << "Index for ( " << x << " , " << y <<" ) = " << index << std::endl;

            int xx = x + offsetX;
            int yy = y + offsetY;
            bool isObstacle;
            bool onMap = NavigationMap::isThereAnObstacle( xx, yy, &isObstacle );
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
        WayPoint* wp = p->pop();
        int n = 1;
        while ( wp )
        {
            int x = wp->x() - offsetX;
            int y = wp->y() - offsetY;

            int index = x + y*maxX;
            data[index] = '0' + (n % 10);
            ++n;

            wp = p->pop();
        }
    }

    // Now overlay the start and goal

    int index = (startX - offsetX) + (startY - offsetY) * maxX;
    data[index] = 'S';

    index = (goalX - offsetX) + (goalY - offsetY) * maxX;
    data[index] = 'G';
}


DisplayMap::~DisplayMap()
{
    delete data;
}


void DisplayMap::display()
{
    // Display the map

    int maxX = NavigationMap::sizeX();
    int maxY = NavigationMap::sizeY();

    std::cout << std::endl << "Display the map..." << std::endl;

    int i = 0;
    int size = maxX * maxY;

    int digit;
    std::cout << ' ';
    for ( int y = 0, digit = 1; y < maxY; ++y, ++digit )
    {
        digit %= 10;
        std::cout << digit;
    }
    std::cout << std::endl;

    for ( int y = 0, digit = 1; y < maxY; ++y, ++digit )
    {
        digit %= 10;
        std::cout << digit;

        for ( int x = 0; x < maxX; ++x )
        {
            int index = x + y * maxY;

            std::cout << data[index];
        }
        std::cout << std::endl;
    }

}




int main()
{
    std::cout << "Testing the path finder algorithm" << std::endl;

    NavigationMap::init();

    setUpNavMap();

//    displayRawMap();

    Path* p = findPath( kStartX, kStartY, kGoalX, kGoalY );



//    Vertex v0( -36, 8, 0, 0 );
//    Vertex v1( -30, 12, 0, 0 );
//    bool a = haveLineOfSight( &v0, &v1 );

    DisplayMap dm( p, kStartX, kStartY, kGoalX, kGoalY );
    dm.display();
#if 0
    if ( a )
    {
        std::cout << "line of sight: YES" << std::endl;
    }
    else
    {
        std::cout << "line of sight: NO" << std::endl;
    }
#endif

    delete p;

    std::cout << std::endl << std::endl;
}





#if MAP1
void setUpNavMap()
{
    // Build a wall from (-25, -30) to (-25, 30)
    for ( int i = -30; i < 31; ++i )
    {
        NavigationMap::markObstacle( -25, i );
    }

    // Build a wall from (-30, 10) to (-20, 10)
    for ( int i = -30; i < -19; ++i )
    {
        NavigationMap::markObstacle( i, 10 );
    }

    // Build a wall from (-10, 0) to (10, 0)
    for ( int i = -10; i < 11; ++i )
    {
        NavigationMap::markObstacle( i, 0 );
    }

    // Build a wall from ( 0, -40) to (0, 10)
    for ( int i = -40; i < 11; ++i )
    {
        NavigationMap::markObstacle( 0, i );
    }

    // Build a wall from (20, -10) to (20, 40)
    for ( int i = -10; i < 41; ++i )
    {
        NavigationMap::markObstacle( 20, i );
    }


    // Build a wall from (20, -5) to (30, -5)
    for ( int i = 20; i < 31; ++i )
    {
        NavigationMap::markObstacle( i, -5 );
    }
}
#endif



#if MAP2
void setUpNavMap()
{
    // Build a wall from (-25, -25) to (-25, 25)
    // And from (0, -25) to (0, 25)
    // and from (25, -25) to (25, 25)
    for ( int i = -25; i < 26; ++i )
    {
        NavigationMap::markObstacle( -25, i );
        NavigationMap::markObstacle( 0, i );
        NavigationMap::markObstacle( 25, i );
    }

    // Build a wall from (-25, -25) to (25, -25)
    // And from (-25, 25) to (25, 25)
    for ( int i = -25; i < 26; ++i )
    {
        NavigationMap::markObstacle( i, -25 );
        NavigationMap::markObstacle( i, 25 );
    }

    // Build a door from ( -25, -20) to (-25, -15)
    for ( int i = -20; i < -14; ++i )
    {
        NavigationMap::markClear( -25, i );
    }

    // Build a door from (15, 25) to (20, 25)
    for ( int i = 15; i < 21; ++i )
    {
        NavigationMap::markClear( i, 25 );
    }

}
#endif



#if MAP3
void setUpNavMap()
{
    // Build a wall from (-25, -25) to (-25, 25)
    // And from (0, -25) to (0, 25)
    // and from (25, -25) to (25, 25)
    for ( int i = -25; i < 26; ++i )
    {
        NavigationMap::markObstacle( -25, i );
        NavigationMap::markObstacle( 0, i );
        NavigationMap::markObstacle( 25, i );
    }

    // Build a wall from (-25, -25) to (25, -25)
    // And from (-25, 25) to (25, 25)
    for ( int i = -25; i < 26; ++i )
    {
        NavigationMap::markObstacle( i, -25 );
        NavigationMap::markObstacle( i, 25 );
    }

    // Build a door from ( -25, 15) to (-25, 20)
    for ( int i = 15; i < 21; ++i )
    {
        NavigationMap::markClear( -25, i );
    }

    // Build a door from (15, 25) to (20, 25)
    for ( int i = 15; i < 21; ++i )
    {
        NavigationMap::markClear( i, 25 );
    }

}
#endif



void displayRawMap()
{
    // Display the map

    std::cout << std::endl << "Display the map..." << std::endl;

    int digit;
    std::cout << ' ';
    for ( int x = NavigationMap::minXCoord(), digit = 1; x < NavigationMap::maxXCoord(); ++x, ++digit )
    {
        digit %= 10;
        std::cout << digit;
    }
    std::cout << std::endl;

    for ( int y = NavigationMap::minYCoord(), digit = 1; y < NavigationMap::maxYCoord(); ++y, ++digit )
    {
        digit %= 10;
        std::cout << digit;

        for ( int x = NavigationMap::minXCoord(); x < NavigationMap::maxXCoord(); ++x )
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



void dumpPath( Path* p )
{
    WayPoint* wp = p->pop();
    while ( wp )
    {
        std::cout << "( " << wp->x() << " , " << wp->y() << " )" << std::endl;
        wp = p->pop();
    }
}


