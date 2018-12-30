/*
    FrontierList.cpp - A priority queue implementation for use in
    implementing the Lazy Theta* path-finding algorithm.  Designed
    to contain the prioritizedlist of frontier (a.k.a. open) vertices.

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



#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD



#include "FrontierList.h"


#if __AVR__

#include "ErrorCodes.h"
#include "ErrorUnrecoverable.h"

#define LINUX_NOTHROW

#else

#define LINUX_NOTHROW               (std::nothrow)

#endif


#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG

#include <iostream>
#include <new>

#endif



void FrontierList::purge()
{
    // Walk down the list and delete all the nodes
    while ( mHead )
    {
        Vertex* next = mHead->next();
        delete mHead;
        mHead = next;
    }
}


int FrontierList::len()
{
    int count = 0;
    Vertex* node = mHead;

    // Walk down the list and count the nodes
    while ( node )
    {
        ++count;
        node = node->next();
    }

    return count;
}



void FrontierList::add( Vertex* v )
{
    // Only add if it isn't null...
    if ( v )
    {
        // Take a precaution
        v->setNext( 0 );

        float priority = v->priority();

        if ( !mHead || priority < mHead->priority() )
        {
            v->setNext( mHead );
            mHead = v;
        }
        else
        {
            // Find where this one goes; smallest up front
            Vertex* addHere = mHead;
            Vertex* next = addHere->next();
            while ( next && next->priority() <= priority )
            {
                addHere = next;
                next = addHere->next();
            }

            v->setNext( next );
            addHere->setNext( v );
        }
    }
}




Vertex* FrontierList::pop()
{
    Vertex* tmp = mHead;

    if ( mHead )
    {
        mHead = mHead->next();
    }

    return tmp;
}




Vertex* FrontierList::find( int x, int y )
{
    // Walk down the list and delete all the nodes
    Vertex* v = mHead;
    while ( v )
    {
        if ( v->x() == x && v->y() == y )
        {
            return v;
        }

        v = v->next();
    }

    return 0;
}


Vertex* FrontierList::remove( int x, int y )
{
    Vertex* v = mHead;

    if ( v->x() == x && v->y() == y )
    {
        // It's the head, so pop it off
        mHead = v->next();
        return mHead;
    }

    Vertex* vNext = v->next();
    while ( vNext )
    {
        if ( vNext->x() == x && vNext->y() == y )
        {
            // Found our guy
            Vertex* thisOne = vNext;

            // Splice:  make v point to the one past our guy: v->next->next
            v->setNext( vNext->next() );
            return thisOne;
        }

        // Move forward a node
        v = v->next();
        vNext = v->next();
    }

    return 0;
}



void FrontierList::add( int x, int y, float g, float pri, Vertex* parent )
{
    Vertex* v = new LINUX_NOTHROW Vertex( x, y, g, pri, parent );

    if ( v )
    {
        add( v );
    }
    else
    {
        // Unrecoverable out of memory here; clear some memory and error out
        purge();

#if __AVR__
        handleUnrecoverableError( kOutOfMemoryError );
#endif

#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG
        std::cerr << "Out of memory in ExploredList" << std::endl;
#endif
    }
}




#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
