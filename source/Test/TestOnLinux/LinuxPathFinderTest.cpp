/*
    LinuxPathFinderTest.cpp - Test the path finding algorithms.

    Copyright (c) 2018 Igor Mikolic-Torreira.  All right reserved.

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




#if kCarrtNavigationMapGridSize != 32
#error "This test requires a 32 x 32 map"
#endif

#define kGoalX          350
#define kGoalY          -100

#define kStartX         -300
#define kStartY         0



using namespace PathFinder;




void setUpNavMap();

void dumpPath( Path* p );

void displayRawMap();




class DisplayMap
{
public:
    DisplayMap( Path* p, int startX, int startY, int goalX, int goalY, const Map& map );
    DisplayMap( const DisplayMap& dm );
    ~DisplayMap();

    void display();

private:

    char* data;
    const Map& mMap;
};



DisplayMap::DisplayMap( Path* p, int startX, int startY, int goalX, int goalY, const Map& map )
: mMap( map )
{
    int minX = mMap.minXCoord();
    int minY = mMap.minYCoord();
//    int maxX = mMap.maxXCoord();
//    int maxY = mMap.maxYCoord();

    int incr = mMap.cmPerGrid();

    int gridSizeX = mMap.sizeGridX();
    int gridSizeY = mMap.sizeGridY();

    // First load the map
    data = static_cast<char*>( malloc( gridSizeX * gridSizeY ) );

    for ( int y = 0; y < gridSizeY; ++y )
    {
        for ( int x = 0; x < gridSizeX; ++x )
        {
            int index = x  + y*gridSizeX;

//            std::cout << "Index for ( " << x << " , " << y <<" ) = " << index << std::endl;

            int xx = x*incr + minX;
            int yy = y*incr + minY;
            bool isObstacle;
            bool onMap = mMap.isThereAnObstacle( xx, yy, &isObstacle );
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
        std::cout << "Path list" << std::endl;

        WayPoint* wp = p->pop();
        int n = 1;
        while ( wp )
        {

            std::cout << n << "\t\t( " << wp->x() << " , " << wp->y() << " )" << std::endl;

            int x = ( wp->x() - minX ) / incr;
            int y = ( wp->y() - minY ) / incr;

            int index = x + y*gridSizeX;
            data[index] = '0' + (n % 10);
            ++n;

            wp = p->pop();
        }

        std::cout << "End path list" << std::endl;
    }

    // Now overlay the start and goal

    int index = ( (startX - minX) + (startY - minY) * gridSizeX ) / incr;
    data[index] = 'S';

    index = ( (goalX - minX) + (goalY - minY) * gridSizeX ) / incr;
    data[index] = 'G';
}



DisplayMap::~DisplayMap()
{
    free( data );
}



void DisplayMap::display()
{
    // Display the map

    int maxX = mMap.sizeGridX();
    int maxY = mMap.sizeGridY();

    std::cout << std::endl << "Display the map..." << std::endl;

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

    Path* p = findPath( kStartX, kStartY, kGoalX, kGoalY, NavigationMap::getLocalMap() );

    DisplayMap dm( p, kStartX, kStartY, kGoalX, kGoalY, NavigationMap::getLocalMap() );
    dm.display();

    delete p;

    std::cout << std::endl << std::endl;
}





void setUpNavMap()
{
    // Build a cube from ( -8, -16 ) to ( 8, 4)
    for ( int i = -200; i < 201; i += 25 )
    {
        for ( int j = -400; j < 126; j+= 25 )
        {
            NavigationMap::markObstacle( i, j );
        }
    }
}




// cppcheck-suppress unusedFunction
void displayRawMap( const Map& map )
{
    // Display the map

    std::cout << std::endl << "Display the map..." << std::endl;

    int digit;
    std::cout << ' ';
    for ( int x = map.minXCoord(), digit = 1; x < map.maxXCoord(); ++x, ++digit )
    {
        digit %= 10;
        std::cout << digit;
    }
    std::cout << std::endl;

    for ( int y = map.minYCoord(), digit = 1; y < map.maxYCoord(); ++y, ++digit )
    {
        digit %= 10;
        std::cout << digit;

        for ( int x = map.minXCoord(); x < map.maxXCoord(); ++x )
        {
            bool isObstacle;
            bool onMap = map.isThereAnObstacle( x, y, &isObstacle );
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


