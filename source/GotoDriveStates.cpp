/*
    GotoDriveStates.cpp - Goto Drive States for CARRT

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


#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD



#include "GotoDriveStates.h"

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/DriveParam.h"
#include "Drivers/Keypad.h"
#include "Drivers/Lidar.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/Motors.h"
#include "Drivers/Sonar.h"

#include "CarrtCallback.h"
#include "ErrorCodes.h"
#include "EventManager.h"
#include "GotoDriveMenuStates.h"
#include "MainProcess.h"
#include "Navigator.h"
#include "NavigationMap.h"
#include "WelcomeMenuStates.h"

#include "PathSearch/Path.h"
#include "PathSearch/PathFinder.h"

#include "Utils/DebuggingMacros.h"




#ifdef CARRT_ENABLE_GOTO_DEBUG

#define GOTO_DEBUG_PRINT( X )           DEBUG_PRINT( X )
#define GOTO_DEBUG_PRINTLN( X )         DEBUG_PRINTLN( X )
#define GOTO_DEBUG_PRINT_P( X )         DEBUG_PRINT_P( X )
#define GOTO_DEBUG_PRINTLN_P( X )       DEBUG_PRINTLN_P( X )

#define GOTO_DEBUG_DUMP_MAP_GLOBAL()    NavigationMap::getGlobalMap().dumpToDebugSerial()
#define GOTO_DEBUG_DUMP_MAP_LOCAL()     NavigationMap::getLocalMap().dumpToDebugSerial()

#define GOTO_DEBUG_BEEP()               DEBUG_BEEP()
#define GOTO_DEBUG_YIELD_MS( X )        DEBUG_YIELD_MS( X )

#else

#define GOTO_DEBUG_PRINT( X )
#define GOTO_DEBUG_PRINTLN( X )
#define GOTO_DEBUG_PRINT_P( X )
#define GOTO_DEBUG_PRINTLN_P( X )
#define GOTO_DEBUG_DUMP_MAP_GLOBAL()
#define GOTO_DEBUG_DUMP_MAP_LOCAL()
#define GOTO_DEBUG_BEEP()
#define GOTO_DEBUG_YIELD_MS( X )

#endif




#ifndef CARRT_ERASE_MAP_AT_EACH_SCAN
// Default to always resetting the map, because our self-navigation is so
// bad we cannot update the map consistently with prior map entries.
#define CARRT_ERASE_MAP_AT_EACH_SCAN        1
#endif


/*
 *
 * Here is the Goto driving scheme with the relevant classes
 *
 * 1. InitiateGotoDriveState
 *      - This is the entry (first) state for a Goto drive
 *      - Stores the goal position in absolute (N, W) coordinates
 *      - Resets the Navigator
 *      - Inititializes a clean NavigationMap
 *      - Switches to the PointTowardsGoalState
 *
 * 2. We then begin a loop with the following states in sequence:
 *
 *      2.1 PointTowardsGoalState
 *          - Rotate to point towards the goal
 *          - Switch to PerformMappingScanState
 *
 *      2.2 PerformMappingScanState
 *          - Execute a scan and update the map
 *          - Switch to DetermineNextWaypointState
 *
 *      2.3 DetermineNextWaypointState
 *          - Figures out the next waypoint in the Goto sequence
 *          - First runs findPath() using the global NavigationMap to get an inital path
 *          - Next finds the furthest waypoint on the initial path that is also on the
 *            local NavigationMap
 *          - Then use that farthest waypoint as a goal and re-runs findPath() using the
 *            local NavigationMap.
 *          - The furthest waypoint on "nearly" a straightline in the second findPath()
 *            becomes the next waypoint
 *          - Switch to the RotateTowardWaypointState
 *
 *      2.4 RotateTowardWaypointState
 *          - Calculates the amount of turn needed to point toward the next waypoint
 *          - Turns CARRT to drive toward the next waypoint
 *          - Switch to DriveToWaypointState
 *
 *      2.5 DriveToWaypointState
 *          - Calculates distance to the next waypoint
 *          - Drives the calculated distance
 *
 *      2.6 FinishedWaypointDriveState
 *          - Announces arrival at this waypoint
 *          - If close enough to goal, ends the loop by switching to the FinishedGotoDriveState
 *          - If not there yet, loops back by switching to the PointTowardsGoalState
 *
 * 3. FinishedGotoDriveState
 *      - Display drive finished message
 *      - Wait for user to hit any button
 *
 *
 *
 * Note that RotateToHeadingState is an implementation state.  It is a base state that factors out
 * common functionality used by PointTowardsGoalState and RotateTowardWaypointState.
 */



namespace
{

    const int   kGlobalCmPerGrid            = 32;
    const int   kLocalCmPerGrid             = 16;

    const float kPi                         = 3.1415926536;
    const float kRadiansToDegrees           = 180.0 / kPi;
    const float kDegreesToRadians           = kPi / 180.0;
    const int   kDirectionAllowanceDeg      = 10;
    const float kDirectionAllowanceRad      = kDirectionAllowanceDeg * kPi / 180.0;

    const float kCriteriaForGoal            = 20.0;                 // cm



    int sGoalX;
    int sGoalY;


    int roundToInt( float x )
    { return static_cast<int>( x >= 0 ? x + 0.5 : x - 0.5 ); }


    float distance( int x1, int y1, int x2, int y2 )
    {
        float dx = static_cast<float>( x1 ) - static_cast<float>( x2 );
        float dy = static_cast<float>( y1 ) - static_cast<float>( y2 );
        return sqrt( dx*dx + dy*dy );
    }
}







//****************************************************************************


namespace
{
    const PROGMEM char sGoalNW[] = "Goal (N, W):";
}


InitiateGotoDriveState::InitiateGotoDriveState( GotoDriveMode mode, int goalAxis1, int goalAxis2 ) :
mMode( mode )
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nStarting GoTo Drive" ) );
    GOTO_DEBUG_PRINTLN_P( PSTR( "*** All coords in cm ***" ) );

    GOTO_DEBUG_PRINT_P( PSTR( "Current heading:  ") );
    GOTO_DEBUG_PRINTLN( Navigator::getCurrentHeading() );

    GOTO_DEBUG_PRINTLN_P( PSTR( "Map scale is:" ) );
    GOTO_DEBUG_PRINT_P( PSTR( "Global " ) );
    GOTO_DEBUG_PRINT( kGlobalCmPerGrid );

    GOTO_DEBUG_PRINT_P( PSTR( " cm/grid;  Local " ) );
    GOTO_DEBUG_PRINT( kLocalCmPerGrid );
    GOTO_DEBUG_PRINTLN_P( PSTR( " cm/grid" ) );

    GOTO_DEBUG_PRINTLN_P( PSTR( "Entered coords:" ) );

    Navigator::reset();

    NavigationMap::init( kGlobalCmPerGrid, kLocalCmPerGrid );

    switch ( mode )
    {
        case kRelative:

            GOTO_DEBUG_PRINTLN_P( PSTR( "Relative" ) );
            GOTO_DEBUG_PRINT_P( PSTR( "X = " ) ); GOTO_DEBUG_PRINTLN( goalAxis1 );
            GOTO_DEBUG_PRINT_P( PSTR( "Y = " ) ); GOTO_DEBUG_PRINTLN( goalAxis2 );
            GOTO_DEBUG_PRINTLN_P( PSTR( "Converted to" ) );

            convertInputsToAbsoluteCoords( goalAxis1, goalAxis2 );
            break;

        case kAbsolute:
            sGoalX = goalAxis1;
            sGoalY = goalAxis2;
            break;
    }

    GOTO_DEBUG_PRINTLN_P( PSTR( "Absolute" ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) ); GOTO_DEBUG_PRINTLN( sGoalX );
    GOTO_DEBUG_PRINT_P( PSTR( "W = " ) ); GOTO_DEBUG_PRINTLN( sGoalY );
}


void InitiateGotoDriveState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sGoalNW );
    Display::setCursor( 1, 0 );
    Display::print( sGoalX );
    Display::setCursor( 1, 8 );
    Display::print( sGoalY );

    mCount = 5;
}


bool InitiateGotoDriveState::onEvent( uint8_t event, int16_t button )
{
    // Provide a count-down before drive begins... (with abort possibility)

    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        // Abort
        MainProcess::changeState( new GotoDriveMenuState );
    }
    else if ( mCount == 0 )
    {
        // If count is over, then switch to next state
        GOTO_DEBUG_YIELD_MS( 500 );
        GOTO_DEBUG_BEEP();
        MainProcess::changeState( new PointTowardsGoalState );
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        // Else continue the count-down
        Beep::beep();
        if ( mCount > 0 )
        {
            --mCount;
        }
    }

    return true;
}


void InitiateGotoDriveState::convertInputsToAbsoluteCoords( int goalAxis1, int goalAxis2 )
{
    Vector2Float coordsGlobal = Navigator::convertRelativeToAbsoluteCoordsCm( goalAxis1, goalAxis2 );

    sGoalX = roundToInt( coordsGlobal.x );
    sGoalY = roundToInt( coordsGlobal.y );
}





//****************************************************************************


namespace
{
    //                                         0123456789012345
    const PROGMEM char sTurnAt[]            = "Now at         T";
};


RotateToHeadingState::RotateToHeadingState( PGM_P topRowLabel ) :
mTopRowLabel( topRowLabel )
{
    // Nothing else to do
}


void RotateToHeadingState::onEntry()
{
    mPriorLeftToGo = 360;

    int currentHeading = Navigator::getCurrentHeading();
    int rotationAngle = mDesiredHeading - currentHeading;

    if ( rotationAngle > 180 )
    {
        rotationAngle -= 360;
    }
    else if ( rotationAngle <= -180 )
    {
        rotationAngle += 360;
    }

    mRotateLeft = ( rotationAngle < 0 );

    Display::clear();
    Display::displayTopRowP16( mTopRowLabel );

    uint8_t col = ( mDesiredHeading >= 100 ? 12 : ( mDesiredHeading >= 10 ? 13 : 14 ) );
    Display::setCursor( 0, col );
    Display::print( mDesiredHeading );

    displayProgress( currentHeading );

    if ( isRotationDone( currentHeading ) )
    {
        // No need to rotate
        GOTO_DEBUG_PRINTLN_P( PSTR( "No need to rotate") );

        doFinishedRotating();
    }
    else
    {
        if ( mRotateLeft )
        {
            Motors::rotateLeft();
        }
        else
        {
            Motors::rotateRight();
        }
        Navigator::movingTurning();
    }
}


void RotateToHeadingState::onExit()
{
    Motors::stop();
    Navigator::stopped();

    Motors::setSpeedAllMotors( Motors::kFullSpeed );

    delete this;
}


bool RotateToHeadingState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        // Abort
        Motors::stop();
        Navigator::stopped();
        MainProcess::changeState( new GotoDriveMenuState );
    }
    else if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        int currentHeading = roundToInt( Navigator::getCurrentHeading() );

        if ( isRotationDone( currentHeading ) )
        {
            Motors::stop();
            Navigator::stopped();

            displayProgress( currentHeading );
            Beep::chirp();
            CarrtCallback::yieldMilliseconds( 3000 );
            GOTO_DEBUG_BEEP();

            doFinishedRotating();
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        int currentHeading = roundToInt( Navigator::getCurrentHeading() );
        displayProgress( currentHeading );
    }

    return true;
}


bool RotateToHeadingState::isRotationDone( int currHeading )
{
    const int kSlowThreshold = 25;
    const int kHeadingThreshold = 5;        // degrees

    int delta = abs( mDesiredHeading - currHeading );
    if ( delta > 180 )
    {
        delta = 360 - delta;
    }

    if ( delta > kHeadingThreshold )
    {
        if ( delta < kSlowThreshold )
        {
            // We are close, so slow down
            Motors::setSpeedAllMotors( Motors::kHalfSpeed );
        }

        if ( delta <= mPriorLeftToGo )
        {
            mPriorLeftToGo = delta;
        }
        else
        {
            // delta got bigger: passed the target heading, so reverse
            reverseDirection();
        }

        return false;
    }
    else
    {
        return true;
    }
}


void RotateToHeadingState::reverseDirection()
{
    Motors::stop();
    if ( mRotateLeft )
    {
        Motors::rotateRight();
        mRotateLeft = false;
    }
    else
    {
        Motors::rotateLeft();
        mRotateLeft = true;
    }
    // Reset left to go...
    mPriorLeftToGo = 360;
}


void RotateToHeadingState::displayProgress( int currHeading )
{
    Display::displayBottomRowP16( sTurnAt );

    uint8_t col = ( currHeading >= 100 ? 12 : ( currHeading >= 10 ? 13 : 14 ) );
    Display::setCursor( 1, col );
    Display::print( currHeading );
}


void RotateToHeadingState::doFinishedRotating()
{
    // Error: should never get here because should be overridden by virtual functions
    // in derived classes
    MainProcess::setErrorState( kUnreachableCodeReached );
}






//****************************************************************************


namespace
{
    //                                         0123456789012345
    const PROGMEM char sGoalAt[]            = "Goal at        T";
};


PointTowardsGoalState::PointTowardsGoalState() :
RotateToHeadingState( sGoalAt )
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nRotating to point towards goal" ) );

    // Figure out heading to goal
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    setDesiredHeading( Navigator::convertToCompassAngle( atan2( sGoalY - origY, sGoalX - origX ) ) );

    GOTO_DEBUG_PRINT_P( PSTR( "Origin:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( origX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( origY );

    GOTO_DEBUG_PRINT_P( PSTR( "Goal:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( sGoalX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( sGoalY );

    GOTO_DEBUG_PRINT_P( PSTR( "Desired heading is  " ) );
    GOTO_DEBUG_PRINTLN( getDesiredHeading() );
}


void PointTowardsGoalState::doFinishedRotating()
{
    MainProcess::changeState( new PerformMappingScanState );
}






//****************************************************************************


namespace
{
    const int8_t kScanLimitLeft         = -80;
    const int8_t kScanLimitRight        = 81;
    const int8_t kScanIncrement         = 2;

    const PROGMEM char sLabelMapping[]  = "Mapping...";
    const PROGMEM char sLabelRng[]      = "Rng = ";
    const PROGMEM char sLabelAngle[]    = "Angle = ";
    const PROGMEM char sUnknown[]       = "Unknown";
};


PerformMappingScanState::PerformMappingScanState()
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nPerforming mapping scan" ) );

    mHeading = Navigator::getCurrentHeading();

#ifdef CARRT_ERASE_MAP_AT_EACH_SCAN
    // Erase any previous map data and start over with a blank map
    NavigationMap::erase();
#endif

    GOTO_DEBUG_PRINT_P( PSTR( "Heading: ") );
    GOTO_DEBUG_PRINTLN( mHeading );
    GOTO_DEBUG_PRINT_P( PSTR( "Current position:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( Navigator::getCurrentPositionCm().x );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( Navigator::getCurrentPositionCm().y );
}


void PerformMappingScanState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sLabelMapping );

    mCurrentSlewAngle = kScanLimitLeft;
    Lidar::slew( mCurrentSlewAngle );

    // Allow time for the servo to slew (this might be a big slew)
    CarrtCallback::yieldMilliseconds( 1000 );
}


void PerformMappingScanState::onExit()
{
    Lidar::slew( 0 );

    delete this;
}


bool PerformMappingScanState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new GotoDriveMenuState );
    }
    else if ( event == EventManager::kOneSecondTimerEvent && (param % 2) == 0 )    // Every 2 secs....
    {
        if ( mCurrentSlewAngle > kScanLimitRight )
        {
            // Done with scan
            Lidar::slew( 0 );

            GOTO_DEBUG_PRINTLN_P( PSTR( "Mapping done" ) );
            GOTO_DEBUG_PRINTLN_P( PSTR( "Global map:" ) );
            GOTO_DEBUG_DUMP_MAP_GLOBAL();
            GOTO_DEBUG_PRINTLN_P( PSTR( "Local map:" ) );
            GOTO_DEBUG_DUMP_MAP_LOCAL();
            GOTO_DEBUG_BEEP();

            MainProcess::changeState( new DetermineNextWaypointState );
        }
        else
        {
            // Slew radar into position for next read
            Lidar::slew( mCurrentSlewAngle );

            // Allow time for the servo to slew (this is a small slew)
            CarrtCallback::yieldMilliseconds( 500 );

            int rng = getAndProcessRange();
            displayAngleRange( rng );
        }

        mCurrentSlewAngle += kScanIncrement;
    }

    return true;
}


int PerformMappingScanState::getAndProcessRange()
{
    int rng;
    int err = Lidar::getMedianDistanceInCm( &rng );

    if ( !err )
    {
        float rad = mCurrentSlewAngle * kDegreesToRadians;
        float cosine = cos( rad );
        float sine = sin( rad );

        // First mark as clear everything between CARRT and the lidar obstacle
        const int rngStepSize = kLocalCmPerGrid / 4;
        for ( int r = rngStepSize; r < rng; r += rngStepSize )
        {
            // Get relative coords
            float xRel = static_cast<float>( r ) * cosine;
            // Extra negative here because using compass headings instead of mathematical angles
            // math_angle = 360 - compass_angle, which puts a negative on sin()
            float yRel = -static_cast<float>( r ) * sine;

            // Convert relative coordinates to absolute and mark as clear on map
            Vector2Float coordsGlobal = Navigator::convertRelativeToAbsoluteCoordsCm( xRel, yRel );
            NavigationMap::markClear( roundToInt( coordsGlobal.x ), roundToInt( coordsGlobal.y ) );
        }

        // Now mark the obstacle
        float xRel = static_cast<float>( rng ) * cosine;
        // Extra negative here because using compass headings instead of mathematical angles
        // math_angle = 360 - compass_angle, which puts a negative on sin()
        float yRel = -static_cast<float>( rng ) * sine;

        // Convert relative coordinates to absolute and mark as obstacle on map
        Vector2Float coordsGlobal = Navigator::convertRelativeToAbsoluteCoordsCm( xRel, yRel );
        NavigationMap::markObstacle( roundToInt( coordsGlobal.x ), roundToInt( coordsGlobal.y ) );
    }
    else
    {
        rng = -1;
    }

    return rng;
}


void PerformMappingScanState::displayAngleRange( int rng )
{
    Display::clear();
    Display::setCursor( 0, 0 );
    Display::printP16( sLabelAngle );
    Display::setCursor( 0, 8 );
    Display::print( mCurrentSlewAngle );
    Display::setCursor( 1, 0 );
    Display::printP16( sLabelRng );
    Display::setCursor( 1, 6 );
    if ( rng == -1 )
    {
        Display::printP16( sUnknown );
    }
    else
    {
        Display::print( rng );
    }
}







//****************************************************************************


namespace
{
    //                                             0123456789012345
    const PROGMEM char sPathFinding[]           = "Finding Path...";
    const PROGMEM char sGlobalPathStage[]       = "...Global Path";
    const PROGMEM char sBestGlobalWayPtStage[]  = "...Best Gbl WP";
    const PROGMEM char sLocalPathStage[]        = "...Local Path";
    const PROGMEM char sLongestDriveStage[]     = "...Longest Drv";
}


DetermineNextWaypointState::DetermineNextWaypointState() :
mPath( 0 )
{
    // Nothing else to do
}


void DetermineNextWaypointState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sPathFinding );

    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    mTransferX = mOrigX = roundToInt( currentPosition.x );
    mTransferY = mOrigY = roundToInt( currentPosition.y );

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

    delete this;
}


bool DetermineNextWaypointState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new GotoDriveMenuState );
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        Display::clearBottomRow();

        // Do a stage of the process every 1 second
        switch ( mProgressStage )
        {
            case kGetGlobalPathStage:
                // Find a path to goal on the global map
                Display::displayBottomRowP16( sGlobalPathStage );
                doGlobalPathStage();
                mProgressStage = kGetBestGlobalWayPointStage;
                break;

            case kGetBestGlobalWayPointStage:
                // Find the furthest waypoint that is still on the local MainProcess
                Display::displayBottomRowP16( sBestGlobalWayPtStage );
                doGetBestGlobalWayPointStage();
                mProgressStage = kGetLocalPathStage;
                break;

            case kGetLocalPathStage:
                // Now Find a path on the local map with last global waypoint on local map as a goal
                Display::displayBottomRowP16( sLocalPathStage );
                doGetLocalPathStage();
                mProgressStage = kGetLongestDriveStage;
                break;

            case kGetLongestDriveStage:
                // Now find the longest straight drive...
                Display::displayBottomRowP16( sLongestDriveStage );
                doGetLongestDriveStage();
                mProgressStage = kDoneStage;
                break;

            case kDoneStage:
                // Done, change state
                GOTO_DEBUG_PRINTLN_P( PSTR( "Done determining next waypoint" ) );
                GOTO_DEBUG_BEEP();
                MainProcess::changeState( new RotateTowardWaypointState( mTransferX, mTransferY ) );
                break;
        }
    }

    return true;
}


void DetermineNextWaypointState::doGlobalPathStage()
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nGlobal path stage..." ) );

    GOTO_DEBUG_PRINT_P( PSTR( "Origin:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mOrigX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mOrigY );

    GOTO_DEBUG_PRINT_P( PSTR( "Goal:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( sGoalX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( sGoalY );

    mPath = PathFinder::findPath( mOrigX, mOrigY, sGoalX, sGoalY, NavigationMap::getGlobalMap() );

    // The first waypoint is the origin, so pop it right away (and discard)
    mPath->pop();

    if ( mPath->isEmpty() )
    {
        GOTO_DEBUG_PRINTLN_P( PSTR( "Failed to find global path" ) );

        MainProcess::setErrorState( kUnableToFindGlobalPath );
    }

    GOTO_DEBUG_PRINT_P( PSTR( "Global path first waypoint:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mPath->getHead()->x() );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mPath->getHead()->y() );
}


void DetermineNextWaypointState::doGetBestGlobalWayPointStage()
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nBest global waypoint stage..." ) );

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

    GOTO_DEBUG_PRINT_P( PSTR( "Best waypoint:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mTransferX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mTransferY );

    mPath->purge();
    delete mPath;
    mPath = 0;
}


void DetermineNextWaypointState::doGetLocalPathStage()
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nLocal path stage..." ) );

    GOTO_DEBUG_PRINT_P( PSTR( "Origin:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mOrigX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mOrigY );

    GOTO_DEBUG_PRINT_P( PSTR( "Goal (= global waypoint):  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mTransferX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mTransferY );

    // Now Find a path on the local map with last global waypoint on local map as a goal
    mPath = PathFinder::findPath( mOrigX, mOrigY, mTransferX, mTransferY, NavigationMap::getLocalMap() );

    // The first waypoint is the origin, so pop it right away (and discard)
    mPath->pop();

    if ( mPath->isEmpty() )
    {
        GOTO_DEBUG_PRINTLN_P( PSTR( "Failed to find local path" ) );

        MainProcess::setErrorState( kUnableToFindLocalPath );
    }

    GOTO_DEBUG_PRINT_P( PSTR( "Local path first waypoint:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mPath->getHead()->x() );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mPath->getHead()->y() );
}


void DetermineNextWaypointState::doGetLongestDriveStage()
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nLongest drive stage..." ) );

    // Now find the longest straight drive...
    // NOTE work in radians in this function (save unneeded conversions to degrees)
    PathFinder::WayPoint* wpLast = mPath->getHead();
    float pathDirection = atan2( wpLast->y() - mOrigY, wpLast->x() - mOrigX );

    PathFinder::WayPoint* wp = wpLast->next();
    while ( wp && fabs( atan2( wp->y() - mOrigY, wp->x() - mOrigX ) - pathDirection ) < kDirectionAllowanceRad )
    {
        wpLast = wp;
        wp = wp->next();
    }

    mTransferX = wpLast->x();
    mTransferY = wpLast->y();

    GOTO_DEBUG_PRINT_P( PSTR( "Longest drive waypoint:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mTransferX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mTransferY );

    mPath->purge();
    delete mPath;
    mPath = 0;
}





//****************************************************************************


namespace
{
    //                                         0123456789012345
    const PROGMEM char sWaypointAt[]        = "WayPoint at    T";
};


RotateTowardWaypointState::RotateTowardWaypointState( int wayPointX, int wayPointY ) :
RotateToHeadingState( sWaypointAt ),
mWayPointX( wayPointX ),
mWayPointY( wayPointY )
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nRotating to point towards waypoint" ) );

    // Figure out target heading
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    setDesiredHeading( Navigator::convertToCompassAngle( atan2( wayPointY - origY, wayPointX - origX ) ) );

    GOTO_DEBUG_PRINT_P( PSTR( "Origin:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( origX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( origY );

    GOTO_DEBUG_PRINT_P( PSTR( "Goal:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( wayPointX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( wayPointY );

    GOTO_DEBUG_PRINT_P( PSTR( "Desired heading is  " ) );
    GOTO_DEBUG_PRINTLN( getDesiredHeading() );
}


void RotateTowardWaypointState::doFinishedRotating()
{
    MainProcess::changeState( new DriveToWaypointState( mWayPointX, mWayPointY ) );
}








//****************************************************************************


namespace
{
    const int kMinDistToObstacle = 25;   // cm

    //                                     0123456789012345
    const PROGMEM char sDriveTo[]       = "Driving       cm";
    const PROGMEM char sDriveSoFar[]    = "So far        cm";
};


DriveToWaypointState::DriveToWaypointState( int wayPointX, int wayPointY ) :
mWayPointX( wayPointX ),
mWayPointY( wayPointY )
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "\nDrive to waypoint" ) );

    // Figure out distance/time to waypoint
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    float distanceToDriveCm = distance( wayPointX, wayPointY, origX, origY );
    mQtrSecondsToDrive = roundToInt( 4 * DriveParam::timeSecAtFullSpeedGivenDistanceCm( distanceToDriveCm ) );
    mDistanceToDriveCm = roundToInt( distanceToDriveCm );

    GOTO_DEBUG_PRINT_P( PSTR( "Origin:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( origX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( origY );

    GOTO_DEBUG_PRINT_P( PSTR( "Drive to:  " ) );
    GOTO_DEBUG_PRINT_P( PSTR( "N = " ) );
    GOTO_DEBUG_PRINT( mWayPointX );
    GOTO_DEBUG_PRINT_P( PSTR( "  W = " ) );
    GOTO_DEBUG_PRINTLN( mWayPointY );

    GOTO_DEBUG_PRINT_P( PSTR( "Distance =  " ) );
    GOTO_DEBUG_PRINT( mDistanceToDriveCm );
    GOTO_DEBUG_PRINTLN_P( PSTR( " cm" ) );

    GOTO_DEBUG_PRINT_P( PSTR( "Time to drive =  " ) );
    GOTO_DEBUG_PRINT( mQtrSecondsToDrive );
    GOTO_DEBUG_PRINTLN_P( PSTR( " (1/4 seconds)" ) );
}


void DriveToWaypointState::onEntry()
{
    mElapsedQtrSeconds = 0;

    if ( !mQtrSecondsToDrive )
    {
        // No driving to do
        GOTO_DEBUG_PRINTLN_P( PSTR( "Got a zero time to drive; exiting this state" ) );

        gotoNextState();
    }
    else if ( Sonar::getSinglePingDistanceInCm() < kMinDistToObstacle )
    {
        // Somehow there is something immediately in front of us -- shouldn't be there, not good
        // Don't attempt recovery because this really shouldn't happen

        GOTO_DEBUG_PRINTLN_P( PSTR( "Abort drive: obstacle immediately present" ) );

        Beep::collisionChime();
        MainProcess::setErrorState( kUnexpectedObstacle );
    }
    else
    {
        Display::clear();
        Display::displayTopRowP16( sDriveTo );

        int dist = roundToInt( DriveParam::distCmAtFullSpeedGivenQuarterSeconds( mElapsedQtrSeconds ) );

        uint8_t col = ( dist >= 1000 ? 9 : ( dist >= 100 ? 10 : ( dist >= 10 ? 11 : 12 ) ) );
        Display::setCursor( 0, col );
        Display::print( dist );

        displayDistance();

        // Don't start driving until the first quarter second event
        // Do this to ensure an accurate count
        mDriving = false;
    }
}


void DriveToWaypointState::onExit()
{
    Motors::stop();

    delete this;
}


bool DriveToWaypointState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        // Abort
        Motors::stop();
        Navigator::stopped();
        MainProcess::changeState( new GotoDriveMenuState );
    }
    else if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        if ( mDriving )
        {
            ++mElapsedQtrSeconds;

            if ( mElapsedQtrSeconds > mQtrSecondsToDrive )
            {
                // Drive done
                Motors::stop();
                Navigator::stopped();

                GOTO_DEBUG_PRINTLN_P( PSTR( "Drive finished normally" ) );
                GOTO_DEBUG_BEEP();

                gotoNextState();
            }
        }
        else
        {
            // Not currently driving; start driving
            Motors::goForward();
            Navigator::movingStraight();
            mDriving = true;
        }

        // Check for obstacles
        // CARRT moves at ~ 35 cm/s

        if ( Sonar::getSinglePingDistanceInCm() < kMinDistToObstacle )
        {
            // Somehow there is something immediately in front of us -- shouldn't be there, not good
            // Emergency stop
            Motors::stop();
            Navigator::stopped();

            Beep::collisionChime();

            GOTO_DEBUG_PRINTLN_P( PSTR( "Stop drive: encountered obstacle" ) );

            // Could also be an error abort, but allow recovery because nav isn't that accurate
            // and sonar not that precise, so very conservative in avoiding collisions
            // MainProcess::setErrorState( kUnexpectedObstacle );
            MainProcess::changeState( new PointTowardsGoalState );
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        displayDistance();
    }

    return true;
}


void DriveToWaypointState::gotoNextState()
{
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    float distanceToGoal = distance( sGoalX, sGoalY, origX, origY );

    if ( fabs( distanceToGoal ) < kCriteriaForGoal )
    {
        GOTO_DEBUG_PRINTLN_P( PSTR( "Drive finished at GOAL!" ) );
        MainProcess::changeState( new FinishedGotoDriveState );
    }
    else
    {
        GOTO_DEBUG_PRINTLN_P( PSTR( "Drive finished at Waypoint" ) );
        MainProcess::changeState( new FinishedWaypointDriveState( mWayPointX, mWayPointY ) );
    }

    GOTO_DEBUG_BEEP();
}


void DriveToWaypointState::displayDistance()
{
    Display::displayBottomRowP16( sDriveSoFar );

    int dist = roundToInt( DriveParam::distCmAtFullSpeedGivenQuarterSeconds( mElapsedQtrSeconds ) );

    uint8_t col = ( dist >= 1000 ? 9 : ( dist >= 100 ? 10 : ( dist >= 10 ? 11 : 12 ) ) );
    Display::setCursor( 1, col );
    Display::print( dist );
}







//****************************************************************************


namespace
{
    //                                     0123456789012345
    const PROGMEM char sArrivedWP[]     = "Arrived waypoint";
};


FinishedWaypointDriveState::FinishedWaypointDriveState( int wayPointX, int wayPointY ) :
mWayPointX( wayPointX ),
mWayPointY( wayPointY ),
mElapsedSeconds( 5 )
{
}


void FinishedWaypointDriveState::onEntry()
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "FinishWaypointDriveState: arrived WP" ) );

    Display::clear();
    Display::displayTopRowP16( sArrivedWP );

    // 0123456789012345
    // N sxxxx W sxxxx

    Display::setCursor( 1, 0 );
    Display::print( 'N' );
    Display::setCursor( 1, 2 );
    Display::print( mWayPointX );
    Display::setCursor( 1, 8 );
    Display::print( 'W' );
    Display::setCursor( 1, 10 );
    Display::print( mWayPointY );
}


bool FinishedWaypointDriveState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        // Abort
        MainProcess::changeState( new GotoDriveMenuState );
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        if ( mElapsedSeconds )
        {
            Beep::readyChime();
            --mElapsedSeconds;
        }
        else
        {
            MainProcess::changeState( new PointTowardsGoalState );
        }
    }

    return true;
}







//****************************************************************************


namespace
{
    //                                     0123456789012345
    const PROGMEM char sArrivedGoal[]   = "Arrived goal!!!";
};


FinishedGotoDriveState::FinishedGotoDriveState() :
mElapsedSeconds( 5 )
{
}


void FinishedGotoDriveState::onEntry()
{
    GOTO_DEBUG_PRINTLN_P( PSTR( "FinishedGotoDriveState: arrived GOAL!" ) );

    Display::clear();
    Display::displayTopRowP16( sArrivedGoal );

    // 0123456789012345
    // N sxxxx W sxxxx

    Display::setCursor( 1, 0 );
    Display::print( 'N' );
    Display::setCursor( 1, 2 );
    Display::print( sGoalX );
    Display::setCursor( 1, 8 );
    Display::print( 'W' );
    Display::setCursor( 1, 10 );
    Display::print( sGoalY );
}


bool FinishedGotoDriveState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        // Abort
        MainProcess::changeState( new GotoDriveMenuState );
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        if ( mElapsedSeconds )
        {
            Beep::readyChime();
            --mElapsedSeconds;
        }
        else
        {
            MainProcess::changeState( new GotoDriveMenuState );
        }
    }

    return true;
}








#endif // CARRT_INCLUDE_GOTODRIVE_IN_BUILD
