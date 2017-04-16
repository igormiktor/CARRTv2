/*
    PathFinder.h - Tools to find a path to the goal.  Implements the
    Lazy Theta* algorithm (with optimizations).

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




#ifndef PathFinder_h
#define PathFinder_h


#include "Path.h"


class Map;


namespace PathFinder
{

    Path* findPath( int hereX, int hereY, int goalX, int goalY, const Map& map );

};




#endif

