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



#include "Path.h"




void Path::purge()
{
    // Walk down the list and delete all the nodes
    while ( mHead )
    {
        WayPoint* next = mHead->next();
        delete mHead;
        mHead = next;
    }
}


int Path::len()
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


void Path::add( int x, int y )
{
    // We always add at the front, because path gets read in reverse

    WayPoint* wp = new WayPoint( x, y, mHead );

    mHead = wp;
}



WayPoint* Path::pop()
{
    WayPoint* wp = mHead;

    if ( mHead )
    {
        mHead = mHead->next();
    }

    return wp;
}


