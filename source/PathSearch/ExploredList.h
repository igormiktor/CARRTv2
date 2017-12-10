/*
    ExploredList.h - A simple list class for use in
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



#ifndef ExploredList_h
#define ExploredList_h



#include "Vertex.h"





class ExploredList
{
public:

    ExploredList()
    : mHead( 0 ) { }

    ~ExploredList()
    { purge(); }

    void purge();

    int len();

    void add( Vertex* v );

    void add( int x, int y, float g, Vertex* parent );

    bool isEmpty()
    { return mHead == 0; }

    Vertex* getHead()
    { return mHead; }

    Vertex* find( int x, int y );


private:

    Vertex* mHead;
};


#endif

