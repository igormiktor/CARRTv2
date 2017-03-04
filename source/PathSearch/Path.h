/*
    Path.h - A path class to convey the path to the goal.

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



#ifndef Path_h
#define Path_h


namespace PathFinder
{

    class WayPoint
    {
    public:

        WayPoint( int x, int y, WayPoint* next )
        : mX( x ), mY( y ), mNext( next ) { }

        int x() const
        { return mX; }

        int y() const
        { return mY; }

        WayPoint* next() const
        { return mNext; }


    private:

        int         mX;
        int         mY;
        WayPoint*   mNext;
    };



    class Path
    {
    public:

        Path()
        : mHead( 0 ) { }

        ~Path()
        { purge(); }

        void purge();

        int len();

        void add( int x, int y );

        WayPoint* pop();

        bool isEmpty()
        { return mHead == 0; }

        WayPoint* getHead()
        { return mHead; }


    private:

        WayPoint*   mHead;
    };


};


#endif

