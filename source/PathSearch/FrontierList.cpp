/*
    FrontierList.cpp - A priority queue implementation for use in
    implementing the Lazy Theta* path-finding algorithm.  Designed
    to contain the prioritizedlist of frontier (a.k.a. open) vertices.

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



#include <iostream>


#include "FrontierList.h"



void FrontierList::purge()
{
    // Walk down the list and delete all the nodes
    while ( mHead )
    {
        VertexPrioritized* next = mHead->next();
        delete mHead;
        mHead = next;
    }
}


int FrontierList::len()
{
    int count = 0;
    VertexPrioritized* node = mHead;

    // Walk down the list and count the nodes
    while ( node )
    {
        ++count;
        node = node->next();
    }

    return count;
}



void FrontierList::add( VertexPrioritized* v )
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
            VertexPrioritized* addHere = mHead;
            VertexPrioritized* next = addHere->next();
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




VertexPrioritized* FrontierList::pop()
{
    VertexPrioritized* tmp = mHead;

    if ( mHead )
    {
        mHead = mHead->next();
    }

    return tmp;
}




VertexPrioritized* FrontierList::find( int x, int y )
{
    // Walk down the list and delete all the nodes
    VertexPrioritized* v = mHead;
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


