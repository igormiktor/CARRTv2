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
void goDown();
void goUp();
void goLeft();
void goRight();
void goUpLeft();
void goUpRight();

int main()
{
    NavigationMap::init();

    int maxX = NavigationMap::sizeX();
    int maxY = NavigationMap::sizeY();

    std::cout << "Map size is " << maxX << " , " << maxY << std::endl;

    loadMap();
    displayMap();
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


    std::cout << std::endl << "Done" << std::endl;
}


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


void loadMap()
{
    // Load the nav map with a pattern

    for ( int x = -30; x < 0; x +=2 )
    {
        int width = 31 + x;
        int halfWidth = width/2;
        for ( int y = -halfWidth; y < halfWidth + 1; ++y )
        {
            bool onMap = NavigationMap::markObstacle( x, y );
            if ( !onMap )
            {
                std::cout << "Went off map at ( " << x << " , " << y << " ) " << std::endl;
            }
        }
    }

    for ( int x = 0; x < 31; x +=2 )
    {
        int width = 31 - x;
        int halfWidth = width/2;
        for ( int y = -halfWidth; y < halfWidth + 1; ++y )
        {
            bool onMap = NavigationMap::markObstacle( x, y );
            if ( !onMap )
            {
                std::cout << "Went off map at ( " << x << " , " << y << " ) " << std::endl;
            }
        }
    }
}





void displayMap()
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




