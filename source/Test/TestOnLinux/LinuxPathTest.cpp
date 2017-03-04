/*
    LinuxPathTest.cpp - Testing harness for the vertex lists
    used to support path finding.

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
#include <string.h>
#include <stdlib.h>
#include <math.h>


#include "PathSearch/Path.h"
#include "PathSearch/ExploredList.h"



void dumpPath( Path* p );

int main()
{
    std::cout << "Testing the path list" << std::endl;

    Path p;

    for ( int x = 0; x < 8; ++x )
    {
        p.add( x, x );
    }

    std::cout << "Length of path: " << p.len() << std::endl;


    dumpPath( &p );

    std::cout << "Length of path: " << p.len() << std::endl;

    std::cout << std::endl << std::endl;
}




void dumpPath( Path* p )
{
    WayPoint* wp = p->pop();
    while ( wp )
    {
        std::cout << "( " << wp->x() << " , " << wp->y() << " )" << std::endl;
        wp = p->pop();
    }
}


