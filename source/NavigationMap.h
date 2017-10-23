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



#ifndef kCarrtNavigationMapGridSize
#define kCarrtNavigationMapGridSize    32
#endif

#ifndef kCarrtNavigationMapGridSizeX
#define kCarrtNavigationMapGridSizeX    kCarrtNavigationMapGridSize
#endif

#ifndef kCarrtNavigationMapGridSizeY
#define kCarrtNavigationMapGridSizeY    kCarrtNavigationMapGridSize
#endif


#if kCarrtNavigationMapGridSizeX % 8
#error "kCarrtNavigationMapGridSizeX must be a multiple of 8"
#endif

#if kCarrtNavigationMapGridSizeY % 8
#error "kCarrtNavigationMapGridSizeY must be a multiple of 8"
#endif


#define kCarrtNavigationMapLogicalSize            ( kCarrtNavigationMapGridSizeX * kCarrtNavigationMapGridSizeY )
#define kCarrtNavigationMapPhysicalSize           ( kCarrtNavigationMapLogicalSize / 8 )
#define kCarrtNavigationMapRowSizeBytes           ( kCarrtNavigationMapGridSizeY / 8 )



// TODO: rounding of nav coordiantes


class Map
{
public:

    Map( int cmPerGrid, int xCenterInCm, int yCenterInCm );

    void reset( int cmPerGrid, int xCenterInCm, int yCenterInCm );

    bool markMap( int navX, int navY, bool isObstacle );

    bool isThereAnObstacle( int navX, int navY, bool* isObstacle ) const;

    bool isThereAnObstacleGridCoords( int gridX, int gridY, bool* isObstacle ) const;

    void recenterMapOnNavCoords( int newNavCenterXinCm, int newNavCenterYinCm, int* preservedXMin, int* preservedXMax, int* preservedYMin, int* preservedYMax );

    bool markObstacle( int navX, int navY )
    {
        return markMap( navX, navY, true );
    }

    bool markClear( int navX, int navY )
    {
        return markMap( navX, navY, false );
    }


    int cmPerGrid() const
    { return mCmPerGrid; }

    void setCmPerGrid( int cmPerGrid );

    int convertToGridX( int xInCm ) const
    { return ( xInCm - mLowerLeftCornerNavX + mHalfCmPerGrid ) / mCmPerGrid; }

    int convertToGridY( int yInCm ) const
    { return ( yInCm - mLowerLeftCornerNavY + mHalfCmPerGrid ) / mCmPerGrid; }

    int convertToNavX( int gridX ) const
    { return mLowerLeftCornerNavX + gridX * mCmPerGrid; }

    int convertToNavY( int gridY ) const
    { return mLowerLeftCornerNavY + gridY * mCmPerGrid; }


    int sizeGridX() const
    { return kCarrtNavigationMapGridSizeX; }

    int sizeGridY() const
    { return kCarrtNavigationMapGridSizeY; }

    int minXCoord() const
    { return convertToNavX( 0 ); }

    int maxXCoord() const
    { return convertToNavX( kCarrtNavigationMapGridSizeX ); }

    int minYCoord() const
    { return convertToNavY( 0 ); }

    int maxYCoord() const
    { return convertToNavY( kCarrtNavigationMapGridSizeY ); }


    unsigned int memorySize() const
    { return kCarrtNavigationMapPhysicalSize; }


#if CARRT_DEBUG_NAVIGATION_MAP

    char* dumpToStr() const;

#endif  // CARRT_DEBUG_NAVIGATION_MAP


private:

    int mCmPerGrid;
    int mHalfCmPerGrid;

    int mLowerLeftCornerNavX;
    int mLowerLeftCornerNavY;

    uint8_t mMap[ kCarrtNavigationMapPhysicalSize ];

    bool getByteAndBitGridCoords( int gridX, int gridY, int* byte, uint8_t* bit ) const;
    void doTotalMapShift( int x, int y );

};






namespace NavigationMap
{

    void init( int globalMapCmPerGrid = 100, int localMapCmPerGrid = 25, int localMapCenterInCmX = 0, int localMapCenterInCmY = 0 );

    bool markObstacle( int navX, int navY );
    bool markClear( int navX, int navY );
    bool isThereAnObstacle( int navX, int navY, bool* isObstacle );

    void recenterLocalMapOnNavCoords( int newLocalMapCenterInCmX, int newLocalMapCenterInCmY );

    const Map& getGlobalMap();
    const Map& getLocalMap();

    const Map* getGlobalMapPtr();
    const Map* getLocalMapPtr();

};


#endif


#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
