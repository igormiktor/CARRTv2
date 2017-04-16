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
#include <stdlib.h>

#include "PathFinderMap.h"

#include "ExploredList.h"
#include "FrontierList.h"




#if CARRT_LINUX_DEBUG_PATHFINDER

#include <iostream>
#include <new>

#define CARRT_DEBUG_PATHFINDER      1
#define LINUX_NOTHROW               (std::nothrow)

#else

#define LINUX_NOTHROW

#endif







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
        const float w = 1.5;

        return g + w*h;
    }


    bool updateDistance( Vertex* v0, Vertex* v1, const Map& map );

    void updateVertex( Vertex* v0, Vertex* v1, int goalX, int goalY, FrontierList* frontier, const Map& map );

    Path* finishedExtractPath( Vertex* v );

    void checkForLineOfSightAndUpdate( Vertex* v, ExploredList* explored, const Map& map );

}






PathFinder::Path* PathFinder::findPath( int startX, int startY, int goalX, int goalY, const Map& map )
{

#if CARRT_DEBUG_PATHFINDER
    // We want to track the size of memory structures
    int maxSizeExploredList = 0;
    int maxSizeFrontierList = 0;
    int maxSizeCombinedLists = 0;
#endif

    // Create the two lists needed
    ExploredList explored;
    FrontierList frontier;

    float h = dist( startX, startY, goalX, goalY );


    Vertex* start = new LINUX_NOTHROW Vertex( startX, startY, 0, priority( 0, h ), 0 );

    if ( !start )
    {
        // TODO do something in AVR case...
#if CARRT_LINUX_DEBUG_PATHFINDER
        std::cout << "findPath: start is null (out of memory)?" << std::endl;
#endif
    }

    frontier.add( start );

    while ( !frontier.isEmpty() )
    {

#if CARRT_DEBUG_PATHFINDER
        // Track the size of memory structures
        int sizeExploredList = explored.len();
        int sizeFrontierList = frontier.len();
        int sizeCombinedLists = sizeExploredList + sizeFrontierList;

        if ( maxSizeExploredList < sizeExploredList )
        {
            maxSizeExploredList = sizeExploredList;
        }

        if ( maxSizeFrontierList < sizeFrontierList )
        {
            maxSizeFrontierList = sizeFrontierList;
        }

        if ( maxSizeCombinedLists < sizeCombinedLists )
        {
            maxSizeCombinedLists = sizeCombinedLists;
        }
#endif

        // Take the best candidate on the border of explored cells
        Vertex* v0 = frontier.pop();

        // Lazy Theta* assumes line of sight, check and update
        checkForLineOfSightAndUpdate( v0, &explored, map );

        // Are we done?
        if ( v0->x() == goalX && v0->y() == goalY )
        {
            Path* pathToGoal = finishedExtractPath( v0 );

#if CARRT_LINUX_DEBUG_PATHFINDER
            std::cout << "Peak explored list size:  " << maxSizeExploredList << std::endl;
            std::cout << "Peak frontier list size:  " << maxSizeFrontierList << std::endl;
            std::cout << "Peak combined list size:  " << maxSizeCombinedLists << std::endl;
#endif

            return pathToGoal;
        }

        // We are exploring this vertex, so add to the explored list
        explored.add( v0 );

        Point neighbors[8];

        int nbrNeighbors = getNeighbors( v0, neighbors, map );

        for ( int i = 0; i < nbrNeighbors; ++i )
        {
            int thisX = neighbors[i].x;
            int thisY = neighbors[i].y;

            if ( !explored.find( thisX, thisY ) )
            {
                Vertex* v1 = frontier.find( thisX, thisY );
                if ( !v1 )
                {
                    float g = v0->g() + dist( v0, thisX, thisY ) + getNearObstaclePenalty( thisX, thisY, map );
                    float pri = priority( g, dist( thisX, thisY, goalX, goalY ) );
                    Vertex* parent = v0->parent();
                    if ( !parent )
                    {
                        parent = v0;
                    }
                    v1 = new LINUX_NOTHROW Vertex( thisX, thisY, g, pri, parent );

                    if ( !v1 )
                    {
                        // TODO do something in AVR case...
#if CARRT_LINUX_DEBUG_PATHFINDER
                        std::cout << "findPath: v1 is null" << std::endl;
#endif
                    }
                }

                updateVertex( v0, v1, goalX, goalY, &frontier, map );
            }
        }
    }

    // No path found
    return 0;
}




void PathFinder::updateVertex( Vertex* v0, Vertex* v1, int goalX, int goalY, FrontierList* frontier, const Map& map )
{
    if ( updateDistance( v0, v1, map ) )
    {
        // We have a better distance. Update the the priority value
        float pri = priority( v1->g(), dist( v1, goalX, goalY ) );
        v1->updatePriority( pri );
    }

    // If the vertex is already on the frontier list,
    // remove it so we can reinsert it with a new priority
    int xTmp = v1->x();
    int yTmp = v1->y();
    if ( frontier->find( xTmp, yTmp ) )
    {
        frontier->remove( xTmp, yTmp );
    }

    // Add vertex to the frontier list with its revised prioriy
    frontier->add( v1 );
}





bool PathFinder::updateDistance( Vertex* v0, Vertex* v1, const Map& map )
{
    Vertex* parentV0 = v0->parent();
    if ( parentV0 )
    {
        float gAlt = parentV0->g() + dist( parentV0, v1 ) + getNearObstaclePenalty( v1, map );

        if ( gAlt < v1->g() )
        {
            v1->updateParent( parentV0 );
            v1->updateG( gAlt );

            return true;
        }
    }

    return false;
}





void PathFinder::checkForLineOfSightAndUpdate( Vertex* v, ExploredList* explored, const Map& map )
{
    const float kBigValue = 1.0e6;


    Vertex* parentOfV = v->parent();
    if ( parentOfV && !haveLineOfSight( parentOfV, v, map ) )
    {
        // If we don't have line of sight, then find which of our
        // neighbors that has been explored provides the shortest path

        Point neighbors[8];
        uint8_t nbrNeighbors = getNeighbors( v, neighbors, map );
        int8_t vNearObstaclePenalty = getNearObstaclePenalty( v, map );

        float minG = kBigValue;
        Vertex* minV = 0;
        for ( uint8_t i = 0; i < nbrNeighbors; ++i )
        {
            int thisX = neighbors[i].x;
            int thisY = neighbors[i].y;
            Vertex* vn = explored->find( thisX, thisY );
            if ( vn )
            {
                float g = vn->g() + dist( vn, v ) + vNearObstaclePenalty;

                if ( g < minG )
                {
                    minG = g;
                    minV = vn;
                }
            }
        }

        v->updateParent( minV );
        v->updateG( minG );
    }
}





PathFinder::Path* PathFinder::finishedExtractPath( Vertex* v )
{
    int n = 0;

    Path* solution = new LINUX_NOTHROW Path;

    if ( !solution )
    {
        // TODO do something in AVR case...
#if CARRT_LINUX_DEBUG_PATHFINDER
        std::cout << "finishedExtractPath: solution is null" << std::endl;
#endif
    }

    while ( v )
    {
        // As we do this, collapse adjacent way points
        while ( v->parent() && abs( v->parent()->x() - v->x() ) <= 1 && abs( v->parent()->y() - v->y() ) <= 1 )
        {
            // Adjacent, skip this one and go with the parent
            v = v->parent();
        }

        ++n;
        solution->add( v->x(), v->y() );
        v = v->parent();
    }

    return solution;
}
