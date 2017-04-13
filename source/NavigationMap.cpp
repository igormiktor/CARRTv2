/*
    NavigationMap.cpp - Maintain an internal navigation map.

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





#include "NavigationMap.h"

#include <string.h>
#include <stdlib.h>



/*

    Map is stored in the standard C format with the last index
    varying most rapidly.  It is stored a a packed array of bits.
    Specifically as an array of kCarrtNavigationMapSizeRealWorldUnitsX rows
    and kCarrtNavigationMapSizeRealWorldUnitsY columns, with each row being
    backed-bytes (each bit corresponds to a different Y-column).
    So each row only has size in bytes:
    kCarrtNavigationMapRowSizeBytes = kCarrtNavigationMapSizeRealWorldUnitsY / 8

    0        y - axis                  Memory address
    ---------------------------------
    |                              |  (1 * kCarrtNavigationMapRowSizeBytes)
 x  | - - - - - - - - - - - - - - -|  (2 * kCarrtNavigationMapRowSizeBytes)
    |   packed-byte row            |
 a  | - - - - - - - - - - - - - - -|
 x  |                              |
 i  |                              |
 s  |                              |
    |                              |
    |                              |
    |------------------------------+ ( kCarrtNavigationMapSizeRealWorldUnitsX * kCarrtNavigationMapRowSizeBytes)
    |                                total size in bytes = kCarrtNavigationMapPhysicalSize


*/



/*
 * The map is stored in a linear array of bytes, that are logically structured as:
 *  - kCarrtNavigationMapSizeRealWorldUnitsX rows of bytes
 *  - Each row consists of (kCarrtNavigationMapSizeRealWorldUnitsY / 8) bytes
 *  - Each bit corresponds to an individual Y-axis point
 */



Map::Map( int cmPerGrid, int xCenterInCm, int yCenterInCm )
{
    reset( cmPerGrid, xCenterInCm, yCenterInCm );
}




void Map::reset( int cmPerGrid, int xCenterInCm, int yCenterInCm )
{
    mCmPerGrid = cmPerGrid;
    mHalfCmPerGrid = cmPerGrid / 2;

    mLowerLeftCornerNavX = xCenterInCm - ( kCarrtNavigationMapGridSizeX * cmPerGrid ) / 2;
    mLowerLeftCornerNavY = yCenterInCm  - ( kCarrtNavigationMapGridSizeY * cmPerGrid ) / 2;

    memset( mMap, 0, kCarrtNavigationMapPhysicalSize );
}




void Map::setCmPerGrid( int cmPerGrid )
{
    // Preserve the center coordinates
    int centerX = mLowerLeftCornerNavX + kCarrtNavigationMapGridSizeX * mCmPerGrid / 2;
    int centerY = mLowerLeftCornerNavY + kCarrtNavigationMapGridSizeY * mCmPerGrid / 2;

    // Compute the new lower left coordinates using the new cmPerGrid value
    mLowerLeftCornerNavX = centerX - kCarrtNavigationMapGridSizeX * cmPerGrid / 2;
    mLowerLeftCornerNavY = centerY - kCarrtNavigationMapGridSizeY * cmPerGrid / 2;

    // Reset grid scale
    mCmPerGrid = cmPerGrid;
    mHalfCmPerGrid = cmPerGrid / 2;

    // Spoil the map...
    // TODO: try to preserve parts of the map perhaps
    memset(  mMap, 0, kCarrtNavigationMapPhysicalSize );
}





bool Map::markObstacle( int navX, int navY )
{
    int byte;
    uint8_t bit;

    if ( getByteAndBit( navX, navY, &byte, &bit ) )
    {
        mMap[ byte ] |= (1 << bit);
        return true;
    }

    return false;
}




bool Map::markClear( int navX, int navY )
{
    int byte;
    uint8_t bit;

    if ( getByteAndBit( navX, navY, &byte, &bit ) )
    {
        mMap[ byte ] &= ~(1 << bit);
        return true;
    }

    return false;
}




bool Map::isThereAnObstacle( int navX, int navY, bool* isObstacle ) const
{
    int byte;
    uint8_t bit;

    if ( getByteAndBit( navX, navY, &byte, &bit ) )
    {
        // We're on the map, return the data
        *isObstacle = mMap[ byte ] & (1 << bit);
        return true;
    }

    return false;
}




bool Map::getByteAndBit( int navX, int navY, int* byte, uint8_t* bit ) const
{
    // Transform from nav to grid coordinates
    int gridX = convertToGridX( navX );
    int gridY = convertToGridY( navY );

    // Check we are on the map
    if (  gridX < 0 || gridX >= kCarrtNavigationMapGridSizeX
        || gridY < 0 || gridY >= kCarrtNavigationMapGridSizeY )
    {
        return false;
    }

    // We're on the map, return the byte and bit
    *byte = gridX * kCarrtNavigationMapRowSizeBytes + gridY / 8;
    *bit  = gridY % 8;

    return true;
}




bool Map::getByteAndBitGridCoords( int gridX, int gridY, int* byte, uint8_t* bit ) const
{
    // Check we are on the map
    if (  gridX < 0 || gridX >= kCarrtNavigationMapGridSizeX
        || gridY < 0 || gridY >= kCarrtNavigationMapGridSizeY )
    {
        return false;
    }

    // We're on the map, return the byte and bit
    *byte = gridX * kCarrtNavigationMapRowSizeBytes + gridY / 8;
    *bit  = gridY % 8;

    return true;
}




void Map::recenterMapOnNavCoords( int newNavCenterX, int newNavCenterY )
{
    // Compute the grid shifts from the nav coords
    int shiftX = ( newNavCenterX - mLowerLeftCornerNavX ) / mCmPerGrid - ( kCarrtNavigationMapGridSizeX / 2 );
    int shiftY = ( newNavCenterY - mLowerLeftCornerNavY ) / mCmPerGrid - ( kCarrtNavigationMapGridSizeY / 2 );

    // Make sure the y shift is a multiple of 8 (next largest in absolute sense)
    shiftY = ( shiftY < 0 ? -1 : 1 ) * ( ( abs( shiftY ) + 7 ) & ~7 );

    // If the move is too big, skip all this
    if ( abs( shiftX ) >= kCarrtNavigationMapGridSizeX || abs(shiftY ) >= kCarrtNavigationMapGridSizeY )
    {
        // We shift out the entire map
        doTotalMapShift( shiftX, shiftY );

        // Nothing more to do
        return;
    }


    // Do the X axis first...

    int sizeOfShift = abs( shiftX ) * kCarrtNavigationMapRowSizeBytes;
    int numBytesToMove = kCarrtNavigationMapPhysicalSize - sizeOfShift;
    if ( shiftX < 0 )
    {
        // Shift map DOWNWARD,
        // so the memory data moves UPWARD from (0,0) to (N,0)
        // so the origin now maps to origX - abs( shiftX )
        int destOffset = sizeOfShift;
        // uint8_t* destination = mMap + destOffset;
        // uint8_t* source = mMap;
        // memmove( destination, source, numBytesToMove );
        memmove( mMap + destOffset, mMap, numBytesToMove );

        // Zero out the "exposed" map
        memset( mMap, 0, sizeOfShift );
    }
    else if ( shiftX > 0 )
    {
        // Shift map to the UPWARD,
        // so the memory data moves DOWNWARD from (0,0) to (-N,0)
        // so the real-world origin now maps to origX + abs( shiftX )
        int srcOffset = sizeOfShift;
        // uint8_t* destination = mMap;
        // uint8_t* source = mMap + srcOffset;
        // memmove( destination, source, numBytesToMove );
        memmove( mMap, mMap + srcOffset, numBytesToMove );

        // Zerp out the "exposed" map
        // uint8_t* endOfMovedData = mMap + numBytesToMove;
        // memset( endOfMovedData, 0, sizeOfShift );
        memset( mMap + numBytesToMove, 0, sizeOfShift );
    }


    // Now do the Y axis

    // How much to shift each column...
    int sizeOfShiftInBytes = abs( shiftY ) / 8;
    numBytesToMove = kCarrtNavigationMapRowSizeBytes - sizeOfShiftInBytes;
    if ( shiftY < 0 )
    {
        // Shifting the map LEFT, row by row
        // so the memory moves RIGHT from (0,0) to (0,N)
        // so the origin now maps to origY - abs( shiftY )
        for ( int i = 0; i < kCarrtNavigationMapGridSizeX; ++i )
        {
            // Shift this column "right"
            int srcOffset = i * kCarrtNavigationMapRowSizeBytes;
            int destOffset = srcOffset + sizeOfShiftInBytes;
            memmove( mMap + destOffset, mMap + srcOffset, numBytesToMove );

            // Zero out the "exposed" part of the row
            memset( mMap + srcOffset, 0, sizeOfShiftInBytes );
        }
    }
    else if ( shiftY > 0 )
    {
        // Shifting the map RIGHT, row by row
        // so the memory moves LEFT from (0,0) to (-N,0)
        // so the origin now maps to origY + abs( shiftY )
        for ( int i = 0; i < kCarrtNavigationMapGridSizeX; ++i )
        {
            // Shift this column "left"
            int destOffset = i * kCarrtNavigationMapRowSizeBytes;
            int srcOffset = destOffset + sizeOfShiftInBytes;
            memmove( mMap + destOffset, mMap + srcOffset, numBytesToMove );

            // Zero out the "exposed" part of the row
            memset( mMap + destOffset + numBytesToMove, 0, sizeOfShiftInBytes );
        }
    }

    // Adjust the origin
    mLowerLeftCornerNavX += shiftX * mCmPerGrid;
    mLowerLeftCornerNavY += shiftY * mCmPerGrid;
}




void Map::doTotalMapShift( int shiftX, int shiftY )
{
    // Just erase the map and reset the origin
    memset(  mMap, 0, kCarrtNavigationMapPhysicalSize );

    mLowerLeftCornerNavX += shiftX * mCmPerGrid;
    mLowerLeftCornerNavY += shiftY * mCmPerGrid;
}






#if CARRT_DEBUG_NAVIGATION_MAP


bool Map::isThereAnObstacleGridCoords( int gridX, int gridY, bool* isObstacle ) const
{
    int byte;
    uint8_t bit;

    if ( getByteAndBitGridCoords( gridX, gridY, &byte, &bit ) )
    {
        // We're on the map, return the data
        *isObstacle = mMap[ byte ] & (1 << bit);
        return true;
    }

    return false;
}




char* Map::dumpToStr() const
{
    // Dump the contents to a string
    int horizontalLen = kCarrtNavigationMapGridSizeX + 2;
    int verticalLen = kCarrtNavigationMapGridSizeY + 1;

    char* outStr = static_cast<char*>( malloc( horizontalLen * verticalLen + 1 ) );

    char* out = outStr;

    int digit;
    *out++ = ' ';
    for ( int x = 0, digit = 1; x < kCarrtNavigationMapGridSizeX; ++x, ++digit )
    {
        digit %= 10;
        *out++ = '0' + digit;
    }
    *out++ = '\n';

    for ( int y = 0, digit = 1; y < kCarrtNavigationMapGridSizeY; ++y, ++digit )
    {
        digit %= 10;
        *out++ = '0' + digit;

        for ( int x = 0, digit = 1; x < kCarrtNavigationMapGridSizeX; ++x )
        {
            bool isObstacle;
            bool onMap = isThereAnObstacleGridCoords( x, y, &isObstacle );
            if ( !onMap )
            {
                *out++ = '!';
            }
            else
            {
                if ( isObstacle )
                {
                    *out++ = '*';
                }
                else
                {
                    *out++ = '.';
                }
            }
        }
        *out++ = '\n';
    }
    *out++ = 0;

    return outStr;
}


#endif  // CARRT_DEBUG_NAVIGATION_MAP





//************************************************************************************




namespace NavigationMap
{

    Map sGlobalMap( 100, 0, 0 );
    Map sLocalMap( 25, 0, 0 );

}




void NavigationMap::init( int localMapCmPerGrid, int localMapCenterInCmX, int localMapCenterInCmY )
{
    sGlobalMap.reset( 100, 0, 0 );
    sLocalMap.reset( localMapCmPerGrid, localMapCenterInCmX, localMapCenterInCmY );
}




bool NavigationMap::markObstacle( int navX, int navY )
{
    // Mark it on both maps
    sLocalMap.markObstacle( navX, navY );

    // Only a problem if not on the global map
    return sGlobalMap.markObstacle( navX, navY );
}




bool NavigationMap::markClear( int navX, int navY )
{
    // Mark it on both maps
    sLocalMap.markClear( navX, navY );

    // Only a problem if not on the global map
    return sGlobalMap.markClear( navX, navY );
}




bool NavigationMap::isThereAnObstacle( int navX, int navY, bool* isObstacle )
{

    // First check the local, higher-resolution map
    bool success = sLocalMap.isThereAnObstacle( navX, navY, isObstacle );

    if ( !success )
    {
        // No luck locally, try global map
        success = sGlobalMap.isThereAnObstacle( navX, navY, isObstacle );
    }

    return success;
}




void NavigationMap::recenterLocalMapOnNavCoords( int newLocalMapCenterInCmX, int newLocalMapCenterInCmY )
{
    sLocalMap.recenterMapOnNavCoords( newLocalMapCenterInCmX, newLocalMapCenterInCmY );
}





const Map& NavigationMap::getGlobalMap()
{
    return sGlobalMap;
}




const Map& NavigationMap::getLocalMap()
{
    return sLocalMap;
}






#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
