/*
    LinuxVertexListsTest.cpp - Testing harness for the vertex lists
    used to support path finding.

    Copyright (c) 2022 Igor Mikolic-Torreira.  All right reserved.

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


#include "PathSearch/FrontierList.h"
#include "PathSearch/ExploredList.h"



void dumpFrontier( FrontierList* fl );

void dumpExplored( ExploredList* el );


int main()
{
    std::cout << "Testing the explored list" << std::endl;

    ExploredList el;

    for ( int x = 0; x < 8; ++x )
    {
        for ( int y = 0; y < 8; ++y )
        {
            float d = sqrt( x*x + y*y );

            Vertex* v = new Vertex( x, y, d, 0-1, 0 );
            el.add( v );
        }
    }

    std::cout << "Size of explored list: " << el.len() << std::endl;

    if ( el.find( 4, 3 ) )
    {
        std::cout << "Successfully found ( 4, 3 )" << std::endl;
    }
    else
    {
        std::cout << "FAILED to find ( 4, 3 )" << std::endl;
    }

    if ( el.find( 10, 3 ) )
    {
        std::cout << "FAILED: found ( 10, 3 )" << std::endl;
    }
    else
    {
        std::cout << "Successfully did not find ( 10, 3 )" << std::endl;
    }

    dumpExplored( &el );

    std::cout << "Size of explored list: " << el.len() << std::endl;

    std::cout << std::endl << std::endl;



    std::cout << "Testing the frontier list" << std::endl;

    FrontierList fl;

    for ( int x = 0; x < 7; ++x )
    {
        for ( int y = 0; y < 7; ++y )
        {
            float d = sqrt( x*x + y*y );

            Vertex* v = new Vertex( x, y, d, 1.0/(d+1), 0 );
            fl.add( v );
        }
    }

    std::cout << "Size of list: " << fl.len() << std::endl;


    if ( fl.find( 1, 1 ) )
    {
        std::cout << "Successfully found ( 1, 1 )" << std::endl;
    }
    else
    {
        std::cout << "FAILED to find ( 1, 1 )" << std::endl;
    }

    if ( fl.find( 3, -1 ) )
    {
        std::cout << "FAILED: found ( 3, -1 )" << std::endl;
    }
    else
    {
        std::cout << "Successfully did not find ( 3, -1 )" << std::endl;
    }

    Vertex* rm = fl.remove( 2, 2 );
    if ( rm )
    {
        if ( rm->x() == 2 && rm->y() == 2 )
        {
            std::cout << "Successfully removed ( 2, 2 )" << std::endl;
        }
        else
        {
            std::cout << "FAILED: removed something but not ( 2, 2)" << std::endl;
        }
    }
    else
    {
        std::cout << "FAILED to lcoate ( 2, 2 ) for removal" << std::endl;
    }

    std::cout << "Size of list after removal: " << fl.len() << std::endl;

    dumpFrontier( &fl );

    std::cout << "Size of list: " << fl.len() << std::endl;

    std::cout << std::endl << "Done" << std::endl;
}




void dumpExplored( ExploredList* el )
{
    Vertex* v = el->getHead();
    while ( v )
    {
        std::cout << "( " << v->x() << " , " << v->y() << " ), " << v->g() << " , " << v->priority() << std::endl;
        v = v->next();
    }
}





void dumpFrontier( FrontierList* fl )
{
    Vertex* v = fl->pop();
    while ( v )
    {
        std::cout << "( " << v->x() << " , " << v->y() << " ), " << v->g() << " , " << v->priority() << std::endl;
        delete v;
        v = fl->pop();
    }
}


