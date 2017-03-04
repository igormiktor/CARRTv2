/*
    ExploredList.cpp - A simple list implementation for use in
    implementing the Lazy Theta* path-finding algorithm.  Designed
    to contain the list of explored vertices.

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




#include "ExploredList.h"




void ExploredList::purge()
{
    // Walk down the list and delete all the nodes
    while ( mHead )
    {
        Vertex* next = mHead->next();
        delete mHead;
        mHead = next;
    }
}


int ExploredList::len()
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


void ExploredList::add( Vertex* v )
{
    // Only add if it isn't null...
    if ( v )
    {
        // Take a precaution
        v->setNext( 0 );

        if ( !mHead )
        {
            mHead = v;
        }
        else
        {
            // Find the last one
            Vertex* addHere = mHead;
            Vertex* next = addHere->next();
            while ( next )
            {
                addHere = next;
                next = addHere->next();
            }

            addHere->setNext( v );
        }
    }
}



Vertex* ExploredList::find( int x, int y )
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


