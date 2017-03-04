/*
    FrontierList.h - A priority queue class for use in
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



#ifndef FrontierList_h
#define FrontierList_h


#include "Vertex.h"



class FrontierList
{
public:

    FrontierList()
    : mHead( 0 ) { }

    ~FrontierList()
    { purge(); }

    void purge();

    int len();

    void add( Vertex* v );

    void add( int x, int y, float g, float pri, Vertex* parent )
    { add( new Vertex( x, y, g, pri, parent ) ); }

    bool isEmpty()
    { return mHead == 0; }

    Vertex* getHead()
    { return mHead; }

    Vertex* pop();

    Vertex* find( int x, int y );

    Vertex* remove( int x, int y );


private:

    Vertex* mHead;
};


#endif
