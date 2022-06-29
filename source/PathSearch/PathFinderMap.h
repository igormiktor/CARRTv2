/*
    PathFinderMap.h - Tools that connect the path finder to the
    navigation map.

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


#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD




#ifndef PathFinderMap_h
#define PathFinderMap_h


#include <inttypes.h>

#include "Vertex.h"


class Map;


namespace PathFinder
{

    struct Point
    {
        int x;
        int y;

        Point( int xx, int yy ) : x( xx ), y( yy ) {}

        Point() : x( 0 ), y( 0 ) {}
    };



    uint8_t getNeighbors( Vertex* v, Point neighbors[], const Map& map );

    bool haveLineOfSight( Vertex* v0, Vertex* v1, const Map& map );

    int8_t getNearObstaclePenalty( int x, int y, const Map& map );

    inline int8_t getNearObstaclePenalty( Vertex* v, const Map& map )
    { return getNearObstaclePenalty( v->x(), v->y(), map ); }

};




#endif


#endif

