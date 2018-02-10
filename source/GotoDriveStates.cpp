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

#include "Drivers/Display.h"
#include "Drivers/Keypad.h"

#include "ErrorCodes.h"
#include "EventManager.h"
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

    const int kGlobalCmPerGrid      = 32;
    const int kLocalCmPerGrid       = 16;

    PathFinder::Path* p1;
    PathFinder::Path* p2;

}



DeterminePathToGoalState::DeterminePathToGoalState( GotoDriveMode mode, int goalAxis1, int goalAxis2 ) :
mGoalX( goalAxis1 ),
mGoalY( goalAxis2 ),
mMode( mode )
{
#if 0
    Navigator::reset();

    NavigationMap::init( kGlobalCmPerGrid, kLocalCmPerGrid );

    p1 = PathFinder::findPath( 0, 0, goalAxis1, goalAxis2, NavigationMap::getGlobalMap() );

    p2 = PathFinder::findPath( 0, 0, goalAxis1, goalAxis2, NavigationMap::getLocalMap() );
#endif
}


void DeterminePathToGoalState::onEntry()
{
    // TODO
    Display::clear();
    Display::displayTopRow( "Mode: " );
    Display::setCursor( 0, 6 );
    if ( mMode == kRelative )
    {
        Display::print( "Rel" );
    }
    else
    {
        Display::print( "N/E" );
    }

    Display::setCursor( 1, 0 );
    Display::print( mGoalX );
    Display::setCursor( 1, 8 );
    Display::print( mGoalY );
}


void DeterminePathToGoalState::onExit()
{
    // TODO
}


bool DeterminePathToGoalState::onEvent( uint8_t event, int16_t param )
{
    // TODO

    if ( event == EventManager::kKeypadButtonHitEvent )
    {
       MainProcess::changeState( new WelcomeState );
    }

    return 0;
}
