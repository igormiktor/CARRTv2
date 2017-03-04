/*
    PathFinder.cpp - Tools to find a path to the goal.  Implements the
    Lazy Theta* algorithm (with optimizations).

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



/***************************************************************************

This module implements the Lazy Theta* with Optimizations algorithm.

See http://aigamedev.com/open/tutorial/lazy-theta-star/

***************************************************************************/


#include "PathFinder.h"

#include <inttypes.h>
#include <math.h>

#include "PathFinderMap.h"

#include "ExploredList.h"
#include "FrontierList.h"




namespace PathFinder
{

    float dist( float x0, float y0, float x1, float y1 )
    {
        float a = x0 - x1;
        float b = y0 - y1;
        return sqrt( a*a + b*b );
    }


    float dist( Vertex* p, Vertex* q )
    {
        float a = p->x() - q->x();
        float b = p->y() - q->y();
        return sqrt( a*a + b*b );
    }


    float dist( Vertex* p, int x, int y )
    {
        float a = p->x() - x;
        float b = p->y() - y;
        return sqrt( a*a + b*b );
    }


    inline float priority( float g, float h )
    {
        const float w = 1.1;

        return g + w*h;
    }


    bool updateDistance( Vertex* v0, Vertex* v1 );

    void updateVertex( Vertex* v0, Vertex* v1, int goalX, int goalY, FrontierList* frontier );



    Path* finishedExtractPath( Vertex* v );

    void checkForLineOfSightAndUpdate( Vertex* v, ExploredList* explored );

}






PathFinder::Path* PathFinder::FindPath( int startX, int startY, int goalX, int goalY )
{
    // Create the two lists needed
    ExploredList explored;
    FrontierList frontier;

    float h = dist( startX, startY, goalX, goalY );

    Vertex* start = new Vertex( startX, startY, 0, priority( 0, h ), 0 );

    frontier.add( start );

    while ( !frontier.isEmpty() )
    {
        // Take the best candidate on the border of explored cells
        Vertex* v0 = frontier.pop();

        // Lazy Theta* assumes line of sight, check and update
        checkForLineOfSightAndUpdate( v0, &explored );

        // Are we done?
        if ( v0->x() == goalX && v0->y() == goalY )
        {
            Path* pathToGoal = finishedExtractPath( v0 );
            return pathToGoal;
        }

        // We are exploring this vertex, so add to the explored list
        explored.add( v0 );

        Point neighbors[8];

        uint8_t nbrNeighbors = getNeighbors( v0, neighbors );

        for ( uint8_t i = 0; i < nbrNeighbors; ++i )
        {
            int thisX = neighbors[i].x;
            int thisY = neighbors[i].y;
            if ( !explored.find( thisX, thisY ) )
            {
                Vertex* v1 = frontier.find( thisX, thisY );
                if ( !v1 )
                {
                    float g = v0->g() + dist( v0, v1 );
                    float pri = priority( g, dist( v1, goalX, goalY ) );
                    v1 = new Vertex( thisX, thisY, g, pri, v0 );
                }

                updateVertex( v0, v1, goalX, goalY, &frontier );
            }
        }
    }

    // No path found
    return 0;

}




void PathFinder::updateVertex( Vertex* v0, Vertex* v1, int goalX, int goalY, FrontierList* frontier )
{
    float gOld = v1->g();

    if ( updateDistance( v0, v1 ) )
    {
        float pri = priority( v1->g(), dist( v1, goalX, goalY ) );
        v1->updatePriority( pri );

        // If the vertex is already on the frontier list, remove it so we
        // can reinsert it with a new priority
        float xTmp = v1->x();
        float yTmp = v1->y();
        if ( frontier->find( xTmp, yTmp ) )
        {
            frontier->remove( xTmp, yTmp );
        }

        // Add vertex to the frontier list with its revised prioriy
        frontier->add( v1 );
    }
}





bool PathFinder::updateDistance( Vertex* v0, Vertex* v1 )
{
    Vertex* parentV0 = v0->parent();
    if ( parentV0 )
    {
        float gAlt = parentV0->g() + dist( parentV0, v1 );
        if ( gAlt < v1->g() )
        {
            v1->updateParent( parentV0 );
            v1->updateG( gAlt );

            return true;
        }
    }

    return false;
}





void PathFinder::checkForLineOfSightAndUpdate( Vertex* v, ExploredList* explored )
{
    Vertex* parentOfV = v->parent();
    if ( parentOfV && !haveLineOfSight( parentOfV, v ) )
    {
        // If we don't have line of sight, then find which of our
        // neighbors that has been explored provides the shortest path

        Point neighbors[8];
        uint8_t nbrNeighbors = getNeighbors( v, neighbors );

        float minG = 1.0e6;
        Vertex* minV = 0;
        for ( uint8_t i = 0; i < nbrNeighbors; ++i )
        {
            int thisX = neighbors[i].x;
            int thisY = neighbors[i].y;
            Vertex* vn = explored->find( thisX, thisY );
            if ( vn )
            {
                float g = vn->g() + dist( vn, v );
                if ( g < minG )
                {
                    minG = g;
                    minV = vn;
                }
            }
        }

        if ( !minV )
        {
            // Something went wrong!!!
        }

        v->updateParent( minV );
        v->updateG( minG );
    }
}



PathFinder::Path* PathFinder::finishedExtractPath( Vertex* v )
{
    Path* solution = new Path;

    while ( v )
    {
        solution->add( v->x(), v->y() );
        v = v->parent();
    }

    return solution;
}
