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



#ifndef kNavigationMapGridSize
#define kNavigationMapGridSize    32
#endif

#ifndef kNavigationMapGridSizeX
#define kNavigationMapGridSizeX    kNavigationMapGridSize
#endif

#ifndef kNavigationMapGridSizeY
#define kNavigationMapGridSizeY    kNavigationMapGridSize
#endif


#if kNavigationMapGridSizeX % 8
#error "kNavigationMapGridSizeX must be a multiple of 8"
#endif

#if kNavigationMapGridSizeY % 8
#error "kNavigationMapGridSizeY must be a multiple of 8"
#endif


#define kNavigationMapLogicalSize            ( kNavigationMapGridSize * kNavigationMapGridSize )
#define kNavigationMapPhysicalSize           ( kNavigationMapLogicalSize / 8 )
#define kNavigationMapRowSizeBytes           ( kNavigationMapGridSizeY / 8 )



// TODO: rounding of nav coordiantes


class Map
{
public:

    Map( int cmPerGrid, int xCenterInCm, int yCenterInCm );

    void reset( int cmPerGrid, int xCenterInCm, int yCenterInCm );

    bool markObstacle( int navX, int navY );
    bool markClear( int navX, int navY );
    bool isThereAnObstacle( int navX, int navY, bool* isObstacle ) const;
    bool isThereAnObstacleGridCoords( int gridX, int gridY, bool* isObstacle ) const;

    void recenterMapOnNavCoords( int newNavCenterXinCm, int newNavCenterYinCm );

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
    { return kNavigationMapGridSizeX; }

    int sizeGridY() const
    { return kNavigationMapGridSizeY; }

    int minXCoord() const
    { return convertToNavX( 0 ); }

    int maxXCoord() const
    { return convertToNavX( kNavigationMapGridSizeX ); }

    int minYCoord() const
    { return convertToNavY( 0 ); }

    int maxYCoord() const
    { return convertToNavY( kNavigationMapGridSizeY ); }


    unsigned int memorySize() const
    { return kNavigationMapPhysicalSize; }

    char* dumpToStr() const;

private:

    int mCmPerGrid;
    int mHalfCmPerGrid;

    int mLowerLeftCornerNavX;
    int mLowerLeftCornerNavY;

    uint8_t mMap[ kNavigationMapPhysicalSize ];

//    void getMemoryCoordinates( int navX, int navY, int* memX, int* memY ) const;
//    void getNavigationCoordinates( int memX, int memY, int* navX, int* navY ) const;
    bool getByteAndBit( int navX, int navY, int* byte, uint8_t* bit ) const;
    bool getByteAndBitGridCoords( int gridX, int gridY, int* byte, uint8_t* bit ) const;
    void doTotalMapShift( int x, int y );
};






namespace NavigationMap
{

    void init();

    bool markObstacle( int navX, int navY );
    bool markClear( int navX, int navY );
    bool isThereAnObstacle( int navX, int navY, bool* isObstacle );

    void recenterMapOnNavCoords( int newNavCenterXinCm, int newNavCenterYinCm );

    const Map& getGlobalMap();
    const Map& getLocalMap();

};


#endif


#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
