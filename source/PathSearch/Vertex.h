/*
    VertexTypes.h - A vertex classes for use in
    implementing the Lazy Theta* path-finding algorithm.

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



#ifndef Vertex_h
#define Vertex_h


class Vertex
{
public:

    Vertex( int x, int y, float g, Vertex* parent )
    : mX( x ), mY( y ), mG (g ), mParent( parent ), mNext( 0 ) { }

    int x() const
    { return mX; }

    int y() const
    { return mY; }

    float g() const
    { return mG; }

    Vertex* parent() const
    { return mParent; }

    Vertex* next() const
    { return mNext; }

    void updateG( float g )
    { mG = g; }

    void updateParent( Vertex* parent )
    { mParent = parent; }

    void setNext( Vertex* n )
    { mNext = n; }


private:

    int         mX;
    int         mY;
    float       mG;
    Vertex*     mParent;
    Vertex*     mNext;
};






class VertexPrioritized
{
public:

    VertexPrioritized( int x, int y, float g, float pri, Vertex* parent )
    : mX( x ), mY( y ), mG (g ), mPriority( pri ), mParent( parent ), mNext( 0 ) { }

    VertexPrioritized( Vertex* v, float pri )
    : mX( v->x() ), mY( v->y() ), mG (v->g() ), mPriority( pri ), mParent( v->parent() ), mNext( 0 ) { }

    int x() const
    { return mX; }

    int y() const
    { return mY; }

    float g() const
    { return mG; }

    float priority() const
    { return mPriority; }

    Vertex* parent() const
    { return mParent; }

    VertexPrioritized* next() const
    { return mNext; }

    void updateG( float g )
    { mG = g; }

    void updateParent( Vertex* parent )
    { mParent = parent; }

    void setNext( VertexPrioritized* n )
    { mNext = n; }


private:

    int                     mX;
    int                     mY;
    float                   mG;
    float                   mPriority;
    Vertex*                 mParent;
    VertexPrioritized*      mNext;
};



#endif
