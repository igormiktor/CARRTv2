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
    Specifically as an array of kNavigationMapSizeRealWorldUnitsX rows
    and kNavigationMapSizeRealWorldUnitsY columns, with each row being
    backed-bytes (each bit corresponds to a different Y-column).
    So each row only has size in bytes:
    kNavigationMapRowSizeBytes = kNavigationMapSizeRealWorldUnitsY / 8

    0        y - axis                  Memory address
    ---------------------------------
    |                              |  (1 * kNavigationMapRowSizeBytes)
 x  | - - - - - - - - - - - - - - -|  (2 * kNavigationMapRowSizeBytes)
    |   packed-byte row            |
 a  | - - - - - - - - - - - - - - -|
 x  |                              |
 i  |                              |
 s  |                              |
    |                              |
    |                              |
    |------------------------------+ ( kNavigationMapSizeRealWorldUnitsX * kNavigationMapRowSizeBytes)
    |                                total size in bytes = kNavigationMapPhysicalSize


*/



/*
 * The map is stored in a linear array of bytes, that are logically structured as:
 *  - kNavigationMapSizeRealWorldUnitsX rows of bytes
 *  - Each row consists of (kNavigationMapSizeRealWorldUnitsY / 8) bytes
 *  - Each bit corresponds to an individual Y-axis point
 */



namespace NavigationMap
{

    uint8_t mOrigX;
    uint8_t mOrigY;

    uint8_t mMap[ kNavigationMapPhysicalSize ];

    void getMemoryCoordinates( int navX, int navY, int* memX, int* memY );
    void getNavigationCoordinates( int memX, int memY, int* navX, int* navY );
    bool getByteAndBit( int x, int y, int* byte, uint8_t* bit );
    void doTotalMapShift( int x, int y );

}



void NavigationMap::init()
{
    mOrigX = kNavigationMapSizeRealWorldUnitsX / 2;
    mOrigY = kNavigationMapSizeRealWorldUnitsY / 2;
    memset(  mMap, 0, kNavigationMapPhysicalSize );
}



void NavigationMap::getMemoryCoordinates( int navX, int navY, int* memX, int* memY )
{
    *memX = navX + mOrigX;
    *memY = navY + mOrigY;
}


void NavigationMap::getNavigationCoordinates( int memX, int memY, int* navX, int* navY )
{
    *navX = memX - mOrigX;
    *navY = memY - mOrigY;
}


bool NavigationMap::markObstacle( int x, int y )
{
    int byte;
    uint8_t bit;
    if ( getByteAndBit( x, y, &byte, &bit ) )
    {
        mMap[ byte ] |= (1 << bit);
        return true;
    }

    return false;
}



bool NavigationMap::markClear( int x, int y )
{
    int byte;
    uint8_t bit;
    if ( getByteAndBit( x, y, &byte, &bit ) )
    {
        mMap[ byte ] &= ~(1 << bit);
        return true;
    }

    return false;
}




bool NavigationMap::isThereAnObstacle( int x, int y, bool* isObstacle )
{
    int byte;
    uint8_t bit;

    if ( getByteAndBit( x, y, &byte, &bit ) )
    {
        // We're on the map, return the data
        *isObstacle = mMap[ byte ] & (1 << bit);
        return true;
    }

    return false;
}




bool NavigationMap::getByteAndBit( int x, int y, int* byte, uint8_t* bit )
{
    // Transform to memory coordinates
    int memX;
    int memY;
    getMemoryCoordinates( x, y, &memX, &memY );

    // Check we are on the map
    if (  memX < 0 || memX >= kNavigationMapSizeRealWorldUnitsX
        || memY < 0 || memY >= kNavigationMapSizeRealWorldUnitsY )
    {
        return false;
    }

    // We're on the map, return the byte and bit
    *byte = memX * kNavigationMapRowSizeBytes + memY / 8;
    *bit  = memY % 8;

    return true;
}




void NavigationMap::recenterMap( int x, int y )
{
    // Make sure the y shift is a multiple of 8 (next largest in absolute sense)
    y = ( y < 0 ? -1 : 1 )*( ( abs( y ) + 7 ) & ~7 );

    // If the move is too big, skip all this
    if ( abs( x ) >= kNavigationMapSizeRealWorldUnitsX || abs( y ) >= kNavigationMapSizeRealWorldUnitsY )
    {
        // We shift out the entire map
        doTotalMapShift( x, y );

        // Nothing more to do
        return;
    }


    // Do the X axis first...

    int sizeOfShift = abs( x ) * kNavigationMapRowSizeBytes;
    int numBytesToMove = kNavigationMapPhysicalSize - sizeOfShift;
    if ( x < 0 )
    {
        // Shift map DOWNWARD,
        // so the memory data moves UPWARD from (0,0) to (N,0)
        // so the origin now maps to origX - x
        int destOffset = sizeOfShift;
        // uint8_t* destination = mMap + destOffset;
        // uint8_t* source = mMap;
        // memmove( destination, source, numBytesToMove );
        memmove( mMap + destOffset, mMap, numBytesToMove );

        // Zero out the "exposed" map
        memset( mMap, 0, sizeOfShift );
    }
    else if ( x > 0 )
    {
        // Shift map to the UPWARD,
        // so the memory data moves DOWNWARD from (0,0) to (-N,0)
        // so the real-world origin now maps to origX - x
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


    // Now do the yY axis

    // How much to shift each column...
    int sizeOfShiftInBytes = abs( y ) / 8;
    numBytesToMove = kNavigationMapRowSizeBytes - sizeOfShiftInBytes;
    if ( y < 0 )
    {
        // Shifting the map LEFT, row by row
        // so the memory moves RIGHT from (0,0) to (N,0)
        // so the origin now maps to origY - y
        for ( int i = 0; i < kNavigationMapSizeRealWorldUnitsX; ++i )
        {
            // Shift this column "right"
            int srcOffset = i * kNavigationMapRowSizeBytes;
            int destOffset = srcOffset + sizeOfShiftInBytes;
            memmove( mMap + destOffset, mMap + srcOffset, numBytesToMove );

            // Zero out the "exposed" part of the row
            memset( mMap + srcOffset, 0, sizeOfShiftInBytes );
        }
    }
    else if ( y > 0 )
    {
        // Shifting the map RIGHT, row by row
        // so the memory moves LEFT from (0,0) to (-N,0)
        // so the origin now maps to origY - y
        for ( int i = 0; i < kNavigationMapSizeRealWorldUnitsX; ++i )
        {
            // Shift this column "left"
            int destOffset = i * kNavigationMapRowSizeBytes;
            int srcOffset = destOffset + sizeOfShiftInBytes;
            memmove( mMap + destOffset, mMap + srcOffset, numBytesToMove );

            // Zero out the "exposed" part of the row
            memset( mMap + destOffset + numBytesToMove, 0, sizeOfShiftInBytes );
        }
    }

    // Adjust the origin
    mOrigX -= x;
    mOrigY -= y;
}





void NavigationMap::doTotalMapShift( int x, int y )
{
    // Just erase the map and reset the origin
    memset(  mMap, 0, kNavigationMapPhysicalSize );

    mOrigX -= x;

    // Make sure the Y axis shift is a multiple of 8 (next largest in absolute sense)
    y = ( y < 0 ? -1 : 1 )*( ( abs( y ) + 7 ) & ~7 );
    mOrigY -= y;
}



#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
