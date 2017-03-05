/*
    PathFinderMap.cpp - Tools that support the implementation the
    Lazy Theta* algorithm (with optimizations).  Specifically all
    functions that reference the actual navigation map are located
    in this module

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



/***************************************************************************

This module implements the line-of-sight algorithm from:

http://aigamedev.com/open/tutorials/theta-star-any-angle-paths/

***************************************************************************/


#include "PathFinderMap.h"

#include <inttypes.h>
#include <math.h>

#include <iostream>

#include "NavigationMap.h"




namespace PathFinder
{

    bool obstacle( int x, int y );

}



bool PathFinder::obstacle( int x, int y )
{
    // Remember this function receives coordinates at double-scale

    if ( x % 2 == 0 )
    {
        if ( y % 2 == 0 )
        {
            // At the center of a grid cell -- check it
            bool obstacle;
            bool isOnMap = NavigationMap::isThereAnObstacle( x/2, y/2, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }
        }
        else
        {
            // On the boundary between two grid cells -- check both
            bool obstacle;
            bool isOnMap = NavigationMap::isThereAnObstacle( x/2, y/2, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }

            isOnMap = NavigationMap::isThereAnObstacle( x/2, y/2 + 1, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }
        }
    }
    else
    {
        if ( y % 2 == 0 )
        {
            // On the boundary between two grid cells -- check both
            bool obstacle;
            bool isOnMap = NavigationMap::isThereAnObstacle( x/2, y/2, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }

            isOnMap = NavigationMap::isThereAnObstacle( x/2 + 1, y/2, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }
        }
        else
        {
            // At the corner of 4 grid cells -- check all of them
            bool obstacle;
            bool isOnMap = NavigationMap::isThereAnObstacle( x/2, y/2, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }

            isOnMap = NavigationMap::isThereAnObstacle( x/2 + 1, y/2, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }

            isOnMap = NavigationMap::isThereAnObstacle( x/2, y/2 + 1, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }

            isOnMap = NavigationMap::isThereAnObstacle( x/2 + 1, y/2 + 1, &obstacle );
            if ( !isOnMap || obstacle )
            {
                return true;
            }
        }
    }

    return false;
}


uint8_t PathFinder::getNeighbors( Vertex* v, Point neighbors[] )
{
    if ( !v )
    {
        std::cout << "getNeighbors: v is null" << std::endl;
    }

    int x = v->x();
    int y = v->y();

    int8_t n = 0;

    // Check all eight possible neighbors
    for ( int8_t i = -1; i < 2; ++i )
    {
        for ( int8_t j = -1; j < 2; ++j )
        {
            if ( i != 0 || j != 0 )
            {
                bool obstacle;
                bool isOnMap = NavigationMap::isThereAnObstacle( x + i, y + j, &obstacle );
                if ( isOnMap && !obstacle )
                {
                    neighbors[ n ].x = x + i;
                    neighbors[ n ].y = y + j;
                    ++n;
                }
            }
        }
    }

    return n;
}


bool PathFinder::haveLineOfSight( Vertex* v0, Vertex* v1 )
{
    // Trick here is we need to check line-of-sight on a resolution twice as high
    // because our vertices are centers, not corners, and we want to drive a path
    // that avoid corners of grid cells with obstacles.

    if ( !v0 )
    {
        std::cout << "haveLineOfSight: v0 is null" << std::endl;
    }

    if ( !v1 )
    {
        std::cout << "haveLineOfSight: v1 is null" << std::endl;
    }


    std::cout << "haveLineOfSight from ( " << v0->x() << " , " << v0->y() << " ) and ( " << v1->x() << " , " << v1->y() << " )?  ";


    int x0 = 2 * v0->x();
    int y0 = 2 * v0->y();
    int x1 = 2 * v1->x();
    int y1 = 2 * v1->y();

    int dx = x1 - x0;
    int dy = y1 - y0;

    int f = 0;

    int sy = 1;
    int sx = 1;

    if ( dy < 0 )
    {
        dy *= -1;
        sy = -1;
    }

    if ( dx < 0 )
    {
        dx *= -1;
        sx = -1;
    }

    if ( dx >= dy )
    {
        while ( x0 != x1 )
        {
            f += dy;

            if ( f >= dx )
            {
                if ( obstacle( x0 + (sx -1)/2, y0 + (sy-1)/2 ) )
                {
                std::cout << "NO 1:  " << x0 + (sx -1)/2 << " , " << y0 + (sy-1)/2 << std::endl;
                std::cout << "ob " << obstacle( x0 + (sx -1)/2, y0 + (sy-1)/2 ) << std::endl;
                std::cout << "ob1 %2 " << (x0 + (sx -1)/2) % 2 << " , " << (y0 + (sy-1)/2 ) % 2
                << " ob1 /2 "  << (x0 + (sx -1)/2) / 2 << " , " << (y0 + (sy-1)/2 ) / 2  << std::endl;
                    return false;
                }
                y0 += sy;
                f -= dx;
            }

            if ( f != 0 && obstacle( x0 + (sx-1)/2, y0 + (sy-1)/2 ) )
            {
                std::cout << "NO 2" << std::endl;
                return false;
            }

            if ( dy == 0 && obstacle( x0 + (sx-1)/2, y0 ) && obstacle( x0 + (sx-1)/2, y0 - 1 ) )
            {
                std::cout << "NO 3" << std::endl;
                return false;
            }

            x0 += sx;
        }
    }
    else
    {
        while ( y0 != y1 )
        {
            f += dx;
            if ( f >= dy )
            {
                if ( obstacle( x0 + (sx -1)/2, y0 + (sy-1)/2 ) )
                {
                    std::cout << "NO 4" << std::endl;
                    return false;
                }
                x0 += sx;
                f -= dy;
            }

            if ( f != 0 && obstacle( x0 + (sx-1)/2, y0 + (sy-1)/2 ) )
            {
                std::cout << "NO 5" << std::endl;
                return false;
            }

            if ( dx == 0 && obstacle( x0, y0 + (sy-1)/2 ) && obstacle( x0 - 1, y0 + (sy-1)/2 ) )
            {
                std::cout << "NO 6:  " << x0 << " , " << y0 + (sy-1)/2 << " , " << x0 - 1 << " , " << y0 + (sy-1)/2 << std::endl;
                std::cout << "ob1 " << obstacle( x0, y0 + (sy-1)/2 ) << " ob2 " << obstacle( x0 - 1, y0 + (sy-1)/2 ) << std::endl;
                std::cout << "ob1 %2 " << x0 % 2 << " , " << (y0 + (sy-1)/2 ) % 2 << " ob1 /2 "  << x0 / 2 << " , " << (y0 + (sy-1)/2 ) / 2  << std::endl;
                return false;
            }

            y0 += sy;
        }
    }

    std::cout << "YES" << std::endl;
    return true;
}
