/*
    DisplayMap.h - Utility class to display the map and found path

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




#ifndef DisplayMap_h
#define DisplayMap_h

// Forward declarations

namespace PathFinder
{
    class Path;
};

class Map;


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


#endif
