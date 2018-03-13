/*
    Path.cpp - A path class to convey the path to the goal.

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



#include "Path.h"




#if __AVR__

#include "ErrorCodes.h"
#include "ErrorUnrecoverable.h"

#define LINUX_NOTHROW

#endif


#if CARRT_LINUX_DEBUG_PATHFINDER

#include <iostream>
#include <new>

#define CARRT_DEBUG_PATHFINDER      1
#define LINUX_NOTHROW               (std::nothrow)

#endif




void PathFinder::Path::purge()
{
    // Walk down the list and delete all the nodes
    while ( mHead )
    {
        WayPoint* next = mHead->next();
        delete mHead;
        mHead = next;
    }
}


int PathFinder::Path::len()
{
    int count = 0;
    WayPoint* node = mHead;

    // Walk down the list and count the nodes
    while ( node )
    {
        ++count;
        node = node->next();
    }

    return count;
}


void PathFinder::Path::add( int x, int y )
{
    // We always add at the front, because path gets read in reverse

    WayPoint* wp = new LINUX_NOTHROW WayPoint( x, y, mHead );

    if ( wp )
    {
        mHead = wp;
    }
    else
    {
        // Unrecoverable out of memory here; clear some memory and error out
        purge();

#if __AVR__
        handleUnrecoverableError( kOutOfMemoryError );
#endif

#if CARRT_LINUX_DEBUG_PATHFINDER
        std::cerr << "Out of memory in PathFinder" << std::endl;
#endif
    }
}



PathFinder::WayPoint* PathFinder::Path::pop()
{
    WayPoint* wp = mHead;

    if ( mHead )
    {
        mHead = mHead->next();
    }

    return wp;
}


#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
