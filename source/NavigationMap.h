/*
    NavigationMap.h - Maintain an internal navigation map.

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




#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD




#ifndef NavigationMap_h
#define NavigationMap_h

#include <inttypes.h>



#ifndef kNavigationMapSizeRealWorldUnits
#define kNavigationMapSizeRealWorldUnits    160
#endif

#ifndef kNavigationMapSizeRealWorldUnitsX
#define kNavigationMapSizeRealWorldUnitsX    kNavigationMapSizeRealWorldUnits
#endif

#ifndef kNavigationMapSizeRealWorldUnitsY
#define kNavigationMapSizeRealWorldUnitsY    kNavigationMapSizeRealWorldUnits
#endif


#if kNavigationMapSizeRealWorldUnitsX % 8
#error "kNavigationMapSizeRealWorldUnitsX must be a multiple of 8"
#endif

#if kNavigationMapSizeRealWorldUnitsY % 8
#error "kNavigationMapSizeRealWorldUnitsY must be a multiple of 8"
#endif


#define kNavigationMapLogicalSize            ( kNavigationMapSizeRealWorldUnitsX * kNavigationMapSizeRealWorldUnitsY )
#define kNavigationMapPhysicalSize           ( kNavigationMapLogicalSize / 8 )
#define kNavigationMapRowSizeBytes           ( kNavigationMapSizeRealWorldUnitsY / 8 )



namespace NavigationMap
{

    void init();

    bool markObstacle( int x, int y );
    bool markClear( int x, int y );
    bool isThereAnObstacle( int x, int y, bool* isObstacle );
    void recenterMap( int x, int y );

    inline int sizeX()
    { return kNavigationMapSizeRealWorldUnitsX; }

    inline int sizeY()
    { return kNavigationMapSizeRealWorldUnitsY; }

    int minXCoord();
    int maxXCoord();
    int minYCoord();
    int maxYCoord();

    inline unsigned int memorySize()
    { return kNavigationMapPhysicalSize; }

};


#endif


#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
