/*
    GotoDriveStates.cpp - Goto Drive States for CARRT

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


#include "GotoDriveStates.h"

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/Keypad.h"

#include "ErrorCodes.h"
#include "EventManager.h"
#include "GotoDriveMenuStates.h"
#include "MainProcess.h"
#include "Navigator.h"
#include "NavigationMap.h"
#include "WelcomeMenuStates.h"

#include "PathSearch/Path.h"
#include "PathSearch/PathFinder.h"




/*
 *
 * Here is the GoTo driving scheme with the relevant classes
 *
 * 1. InitiateGoToDriveState
 *      - This is the entry (first) state for a GoTo drive
 *      - Stores the goal position in absolute (N, E) coordinates
 *      - Resets the Navigator
 *      - Inititializes a clean NavigationMap
 *      - Swithes to the DetermineNextWaypointState
 *
 * 2. We then begin a loop with the following states in sequence:
 *
 *      2.1 DetermineNextWaypointState
 *          - Figures out the next waypoint in the GoTo sequence
 *          - First runs findPath() using the global NavigationMap to get an inital path
 *          - Next finds the furthest waypoint on the initial path that is also on the
 *            local NavigationMap
 *          - Then use that farthest waypoint as a goal and re-runs findPath() using the
 *            local NavigationMap.
 *          - The furthest waypoint on "nearly" a straightline in the second findPath()
 *            becomes the next waypoint
 *          - Switches to the RotateTowardWaypointState
 *
 *      2.2 RotateTowardWaypointState
 *          - Calculates the amount of turn needed to point toward the next waypoint
 *          - Turns CARRT to drive toward the next waypoint
 *          - Switches to DriveToWaypointState
 *
 *      2.3 DriveToWaypointState
 *          - Calculates distance to the next waypoint
 *          - Drives the calculated distance
 *          - If close enough to goal, ends the loop by switching to the FinishedGoToDriveState
 *          - If not there yet, loops back by switching to the DetermineNextWaypointState
 *
 * 3. FinishedGoToDriveState
 *      - Display drive finished message
 *      - Wait for user to hit any button
 *
 */



namespace
{

    const int kGlobalCmPerGrid          = 32;
    const int kLocalCmPerGrid           = 16;

    const float kPi                     =3.1415926536;
    const float kDirectionAllowance     = 10.0 * kPi / 180.0;

    int sGoalX;
    int sGoalY;


    int roundToInt( float x )
    { return static_cast<int>( x >= 0 ? x + 0.5 : x - 0.5 ); }

}



InitiateGoToDriveState::InitiateGoToDriveState( GotoDriveMode mode, int goalAxis1, int goalAxis2 ) :
mMode( mode )
{
    Navigator::reset();

    NavigationMap::init( kGlobalCmPerGrid, kLocalCmPerGrid );

    switch ( mode )
    {
        case kRelative:
            convertInputsToAbsoluteCoords( goalAxis1, goalAxis2 );
            break;

        case kAbsolute:
            sGoalX = goalAxis1;
            sGoalY = goalAxis2;
            break;
    }
}


namespace
{
    const PROGMEM char sGoalNE[] = "Goal (N, E):";
}


void InitiateGoToDriveState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sGoalNE );
    Display::setCursor( 1, 0 );
    Display::print( sGoalX );
    Display::setCursor( 1, 8 );
    Display::print( sGoalY );

    mCount = 5;
}


void InitiateGoToDriveState::onExit()
{
    // TODO
}


bool InitiateGoToDriveState::onEvent( uint8_t event, int16_t param )
{
    // Provide a count-down before drive begins... (with abort possibility)
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        Beep::beep();
        if ( mCount > 0 )
        {
            --mCount;
        }
        else
        {
            MainProcess::changeState( new DetermineNextWaypointState );
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new GotoDriveMenuState );
    }

    return true;
}


void InitiateGoToDriveState::convertInputsToAbsoluteCoords( int goalAxis1, int goalAxis2 )
{
    Vector2Float coordsGlobal = Navigator::convertRelativeToAbsoluteCoords( goalAxis1, goalAxis2 );

    sGoalX = roundToInt( coordsGlobal.x );
    sGoalY = roundToInt( coordsGlobal.y );
}







//****************************************************************************


DetermineNextWaypointState::DetermineNextWaypointState()
{

}


namespace
{
                                    //   1234567890123456
    const PROGMEM char sPathFinding[] = "Finding Path...";
}


void DetermineNextWaypointState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sPathFinding );

    Vector2Float currentPosition = Navigator::getCurrentPosition();
    mOrigX = roundToInt( currentPosition.x );
    mOrigY = roundToInt( currentPosition.y );

    mProgressStage = kGetGlobalPathStage;

    mPath = 0;
}


void DetermineNextWaypointState::onExit()
{
    if ( mPath )
    {
        mPath->purge();
        delete mPath;
        mPath = 0;
    }
}


bool DetermineNextWaypointState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        doUpdateDisplay();

        // Do a stage of the process every 1/4 second
        switch ( mProgressStage )
        {
            case kGetGlobalPathStage:
                // Find a path to goal on the global map
                doGlobalPathStage();
                mProgressStage = kGetBestGlobalWayPointStage;
                break;

            case kGetBestGlobalWayPointStage:
                // Find the furthest waypoint that is still on the local MainProcess
                doGetBestGlobalWayPointStage();
                mProgressStage = kGetLocalPathStage;
                break;

            case kGetLocalPathStage:
                // Now Find a path on the local map with last global waypoint on local map as a goal
                doGetLocalPathStage();
                mProgressStage = kGetLongestDriveStage;
                break;

            case kGetLongestDriveStage:
                // Now find the longest straight drive...
                doGetLongestDriveStage();
                mProgressStage = kDoneStage;
                break;

            case kDoneStage:
                // Done, change state
                MainProcess::changeState( new RotateTowardWaypointState( mTransferX, mTransferY ) );
                break;
        }
    }

    return true;
}


namespace
{
                                                // 1234567890123456
    const PROGMEM char sGlobalPathStage[]       = "...Global Path";
    const PROGMEM char sBestGlobalWayPtStage[]  = "...Best Gbl WP";
    const PROGMEM char sLocalPathStage[]        = "...Local Path";
    const PROGMEM char sLongestDriveStage[]     = "...Longest Drv";
}


void DetermineNextWaypointState::doUpdateDisplay()
{
    Display::clearBottomRow();
    switch ( mProgressStage )
    {
        case kGetGlobalPathStage:
            // Find a path to goal on the global map
            Display::displayBottomRowP16( sGlobalPathStage );
            break;

        case kGetBestGlobalWayPointStage:
            // Find the furthest waypoint that is still on the local MainProcess
            Display::displayBottomRowP16( sBestGlobalWayPtStage );
            break;

        case kGetLocalPathStage:
            // Now Find a path on the local map with last global waypoint on local map as a goal
            Display::displayBottomRowP16( sLocalPathStage );
            break;

        case kGetLongestDriveStage:
            // Now find the longest straight drive...
            Display::displayBottomRowP16( sLongestDriveStage );
            break;

        case kDoneStage:
            // Done
            break;
    }
}


void DetermineNextWaypointState::doGlobalPathStage()
{
    mPath = PathFinder::findPath( mOrigX, mOrigY, sGoalX, sGoalY, NavigationMap::getGlobalMap() );

    if ( mPath->isEmpty() )
    {

        // TODO error
    }
}


void DetermineNextWaypointState::doGetBestGlobalWayPointStage()
{
    PathFinder::WayPoint* lastW = mPath->getHead();
    const Map& localMap = NavigationMap::getLocalMap();
    if ( localMap.isOnMap( lastW->x(), lastW->y() ) )
    {
        // If first waypoint is on the local map, find the furthest
        // waypoint that is still on the local map

        PathFinder::WayPoint* w = lastW->next();
        while ( w && localMap.isOnMap( w->x(), w->y() ) )
        {
            lastW = w;
            w = w->next();
        }
    }

    mTransferX = lastW->x();
    mTransferY = lastW->y();

    mPath->purge();
    delete mPath;
    mPath = 0;
}


void DetermineNextWaypointState::doGetLocalPathStage()
{
    // Now Find a path on the local map with last global waypoint on local map as a goal
    mPath = PathFinder::findPath( mOrigX, mOrigY, mTransferX, mTransferY, NavigationMap::getLocalMap() );

    if ( mPath->isEmpty() )
    {

        // TODO error
    }
}


void DetermineNextWaypointState::doGetLongestDriveStage()
{
    // Now find the longest straight drive...
    PathFinder::WayPoint* wpLast = mPath->getHead();
    float pathDirection = atan2( wpLast->y() - mOrigY, wpLast->x() - mOrigX );

    PathFinder::WayPoint* wp = wpLast->next();
    while ( wp && fabs( atan2( wp->y() - mOrigY, wp->x() - mOrigX ) - pathDirection ) < kDirectionAllowance )
    {
        wpLast = wp;
        wp = wp->next();
    }

    mTransferX = wpLast->x();
    mTransferY = wpLast->y();

    mPath->purge();
    delete mPath;
    mPath = 0;
}



#if 0
bool DetermineNextWaypointState::onEvent( uint8_t event, int16_t param )
{
    Vector2Float currentPosition = Navigator::getCurrentPosition();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    PathFinder::Path* globalPath = PathFinder::findPath( origX, origY, sGoalX, sGoalY, NavigationMap::getGlobalMap() );

    if ( globalPath->isEmpty() )
    {
        // TODO error
    }

    PathFinder::WayPoint* lastW = globalPath->getHead();
    const Map& localMap = NavigationMap::getLocalMap();
    if ( !localMap.isOnMap( lastW->x(), lastW->y() ) )
    {
        // Then this is our waypoint...
        MainProcess::changeState( new RotateTowardWaypointState( lastW->x(), lastW->y() ) );
    }

    // Find the furthest waypoint that is still on the local MainProcess

    PathFinder::WayPoint* w = lastW->next();
    while ( w && localMap.isOnMap( w->x(), w->y() ) )
    {
        lastW = w;
        w = w->next();
    }

    // Now Find a path on the local map with lastW as a goal
    PathFinder::Path* localPath = PathFinder::findPath( origX, origY, lastW->x(), lastW->y(), NavigationMap::getLocalMap() );

    // Now find the longest straight drive...
    PathFinder::WayPoint* wpLast = localPath->getHead();
    int deltaX = wpLast->x() - origX;
    int deltaY = wpLast->y() - origY;
    float pathDirection = atan2( deltaY, deltaX );

    PathFinder::WayPoint* wp = wpLast->next();
    while ( wp && fabs( atan2( wp->y() - origY, wp->x() - origX ) - pathDirection ) < kDirectionAllowance )
    {
        wpLast = wp;
        wp = wp->next();
    }

    MainProcess::changeState( new RotateTowardWaypointState( wpLast->x(), wpLast->y() ) );

    return true;
}
#endif






//****************************************************************************


RotateTowardWaypointState::RotateTowardWaypointState( int wayPointX, int wayPointY )
{

}


void RotateTowardWaypointState::onEntry()
{
}


void RotateTowardWaypointState::onExit()
{
}


bool RotateTowardWaypointState::onEvent( uint8_t event, int16_t param )
{
    return true;
}





