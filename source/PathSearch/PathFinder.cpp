/*
    PathFinder.cpp - Tools to find a path to the goal.  Implements the
    Lazy Theta* algorithm (with optimizations).

    Copyright (c) 2020 Igor Mikolic-Torreira.  All right reserved.

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



#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD



/***************************************************************************

This module implements the Lazy Theta* with Optimizations algorithm.

See http://aigamedev.com/open/tutorial/lazy-theta-star/

***************************************************************************/


#include "PathFinder.h"

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "ErrorCodes.h"

#include "NavigationMap.h"

#include "PathFinderMap.h"

#include "ExploredList.h"
#include "FrontierList.h"


#if __AVR__

#include "ErrorUnrecoverable.h"

#include "Drivers/Beep.h"
#include "Drivers/Display.h"

#include "AVRTools/MemUtils.h"
#include "AVRTools/SystemClock.h"

#define LINUX_NOTHROW

#else

#define LINUX_NOTHROW               (std::nothrow)

#endif



#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG

#include <iostream>
#include <new>

#endif



#if CARRT_ENABLE_AVR_PATHFINDER_DEBUG

#include "Utils/DebuggingMacros.h"

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



    Path* findPathOnGrid( int startX, int startY, int goalX, int goalY, const Map& map );

    bool updateDistance( Vertex* v0, Vertex* v1, const Map& map );

    void updateVertex( Vertex* v0, Vertex* v1, int goalX, int goalY, FrontierList* frontier, const Map& map );

    Path* finishedExtractPath( Vertex* v, ExploredList* el, FrontierList* fl, const Map& map );

    void checkForLineOfSightAndUpdate( Vertex* v, ExploredList* explored, const Map& map );


#if __AVR__

    void handleCarrtOutOfMemoryError( ExploredList* el, FrontierList* fl )
    {
        // Clear some memory -- we really can't recover from this error, so okay to toss everything
        el->purge();
        fl->purge();

        handleUnrecoverableError( kOutOfMemoryError );
    }

#define doOutOfMemory( X, Y )       handleCarrtOutOfMemoryError( X, Y );

#else

#define doOutOfMemory( X, Y )

#endif

}






PathFinder::Path* PathFinder::findPath( int startX, int startY, int goalX, int goalY, const Map& map )
{
    // Need to convert inputs to grid coords
    int gridStartX = map.convertToGridX( startX );
    int gridStartY = map.convertToGridY( startY );

    int gridGoalX = map.convertToGridX( goalX );
    int gridGoalY = map.convertToGridY( goalY );

    // Perform the path search on the raw grid
    Path* path = findPathOnGrid( gridStartX, gridStartY, gridGoalX, gridGoalY, map );

    // Convert path to navigation coordinates
    if ( path )
    {
        WayPoint* node = path->getHead();

        // Walk down the list and count the nodes
        while ( node )
        {
            int gridX = node->x();
            int gridY = node->y();
            int navX = map.convertToNavX( gridX );
            int navY = map.convertToNavY( gridY );
            node->update( navX, navY );

            node = node->next();
        }
    }

    return path;
}






PathFinder::Path* PathFinder::findPathOnGrid( int startX, int startY, int goalX, int goalY, const Map& map )
{

#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG || CARRT_ENABLE_AVR_PATHFINDER_DEBUG
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
#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG
        std::cerr << "findPath: start is null (out of memory)?" << std::endl;
#elif CARRT_ENABLE_AVR_PATHFINDER_DEBUG
        // Don't use program memory because may not have room to copy it out
        DEBUG_PRINTLN( "start is null" );
#endif

        doOutOfMemory( &explored, &frontier );
    }

    frontier.add( start );

    while ( !frontier.isEmpty() )
    {

#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG || CARRT_ENABLE_AVR_PATHFINDER_DEBUG
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
            Path* pathToGoal = finishedExtractPath( v0, &explored, &frontier, map );

#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG
            std::cerr << "Peak explored list size:  " << maxSizeExploredList << std::endl;
            std::cerr << "Peak frontier list size:  " << maxSizeFrontierList << std::endl;
            std::cerr << "Peak combined list size:  " << maxSizeCombinedLists << std::endl;
#elif CARRT_ENABLE_AVR_PATHFINDER_DEBUG
            DEBUG_PRINT_P( PSTR( "Peak explored list size:  " ) );    DEBUG_PRINTLN( maxSizeExploredList );
            DEBUG_PRINT_P( PSTR( "Peak frontier list size:  " ) );    DEBUG_PRINTLN( maxSizeFrontierList );
            DEBUG_PRINT_P( PSTR( "Peak combined list size:  " ) );    DEBUG_PRINTLN( maxSizeCombinedLists );
            DEBUG_PRINT_P( PSTR( "Peak memory demand:  " ) );         DEBUG_PRINTLN( maxSizeCombinedLists * sizeof( Vertex ) );
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
#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG
                        std::cerr << "findPath: v1 is null" << std::endl;
#elif CARRT_ENABLE_AVR_PATHFINDER_DEBUG
                        // Don't use program memory because may not have memory to copy into SRAM
                        DEBUG_PRINTLN( "v1 is null" );
#endif

                        doOutOfMemory( &explored, &frontier );
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





PathFinder::Path* PathFinder::finishedExtractPath( Vertex* v, ExploredList* el, FrontierList* fl, const Map& map )
{
    int n = 0;

    Path* solution = new LINUX_NOTHROW Path;

    if ( !solution )
    {
#if CARRT_ENABLE_LINUX_PATHFINDER_DEBUG
        std::cerr << "finishedExtractPath: solution is null" << std::endl;
#elif CARRT_ENABLE_AVR_PATHFINDER_DEBUG
        // Don't use program memory because may not have memory to copy into SRAM
        DEBUG_PRINTLN( "soln is null" );
#endif

        doOutOfMemory( el, fl );
    }

    // Always add the final vertex
    solution->add( v->x(), v->y() );
    Vertex* vLastAdded = v;
    v = v->parent();
    ++n;

    while ( v )
    {
        // As we do this, collapse excess way points
        while ( v->parent() && haveLineOfSight( v->parent(), vLastAdded, map ) )
        {
            // Line of sight to parent, so skip this one and go with the parent
            v = v->parent();
        }

        ++n;
        solution->add( v->x(), v->y() );
        vLastAdded = v;
        v = v->parent();
    }

    return solution;
}


#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD || CARRT_INCLUDE_NAVMAP_IN_BUILD
