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

#include "ErrorCodes.h"

#include "Navigator.h"
#include "NavigationMap.h"

#include "PathSearch/Path.h"
#include "PathSearch/PathFinder.h"



/*
DeterminePathToGoalState : public State
{
public:

    DeterminePathToGoalState();
    DeterminePathToGoalState( GotDriveMode mode, int goalAxis1, int goalAxis2 );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    int mGoalX;
    int mGoalY;
};
*/

namespace
{

    const int kGlobalCmPerGrid      = 32;
    const int kLocalCmPerGrid       = 16;

    PathFinder::Path* p1;
    PathFinder::Path* p2;

}



DeterminePathToGoalState::DeterminePathToGoalState( GotoDriveMode mode, int goalAxis1, int goalAxis2 )
{
    Navigator::reset();

    NavigationMap::init( kGlobalCmPerGrid, kLocalCmPerGrid );

    p1 = PathFinder::findPath( 0, 0, goalAxis1, goalAxis2, NavigationMap::getGlobalMap() );

    p2 = PathFinder::findPath( 0, 0, goalAxis1, goalAxis2, NavigationMap::getLocalMap() );
}


void DeterminePathToGoalState::onEntry()
{
    // TODO
}


void DeterminePathToGoalState::onExit()
{
    // TODO
}


bool DeterminePathToGoalState::onEvent( uint8_t event, int16_t param )
{
    // TODO

    return 0;
}
