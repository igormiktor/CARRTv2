/*
    GotoDriveStates.cpp - Goto Drive States for CARRT

    Copyright (c) 2018 Igor Mikolic-Torreira.  All right reserved.

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

#else

#define GOTO_DEBUG_PRINT( X )
#define GOTO_DEBUG_PRINTLN( X )

#endif



/*
 *
 * Here is the Goto driving scheme with the relevant classes
 *
 * 1. InitiateGotoDriveState
 *      - This is the entry (first) state for a Goto drive
 *      - Stores the goal position in absolute (N, E) coordinates
 *      - Resets the Navigator
 *      - Inititializes a clean NavigationMap
 *      - Swithes to the DetermineNextWaypointState
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
 *          - Switches to the RotateTowardWaypointState
 *
 *      2.4 RotateTowardWaypointState
 *          - Calculates the amount of turn needed to point toward the next waypoint
 *          - Turns CARRT to drive toward the next waypoint
 *          - Switches to DriveToWaypointState
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

}






//****************************************************************************


namespace
{
    const PROGMEM char sGoalNE[] = "Goal (N, E):";
}


InitiateGotoDriveState::InitiateGotoDriveState( GotoDriveMode mode, int goalAxis1, int goalAxis2 ) :
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


void InitiateGotoDriveState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sGoalNE );
    Display::setCursor( 1, 0 );
    Display::print( sGoalX );
    Display::setCursor( 1, 8 );
    Display::print( sGoalY );

    mCount = 5;
}


bool InitiateGotoDriveState::onEvent( uint8_t event, int16_t param )
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
            MainProcess::changeState( new PointTowardsGoalState );
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new GotoDriveMenuState );
    }

    return true;
}


void InitiateGotoDriveState::convertInputsToAbsoluteCoords( int goalAxis1, int goalAxis2 )
{
    Vector2Float coordsGlobal = Navigator::convertRelativeToAbsoluteCoords( goalAxis1, goalAxis2 );

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
    // Rotate based on compass, as compass is reliable when rotating in a fixed position

    mPriorLeftToGo = 360;

    int rotationAngle = mDesiredHeading - roundToInt( LSM303DLHC::getHeading() );
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

    displayProgress( roundToInt( LSM303DLHC::getHeading() ) );

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


void RotateToHeadingState::onExit()
{
    Motors::stop();
    Navigator::stopped();

    Motors::setSpeedAllMotors( Motors::kFullSpeed );

    delete this;
}


bool RotateToHeadingState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        int currentHeading = roundToInt( LSM303DLHC::getHeading() );

        if ( isRotationDone( currentHeading ) )
        {
            Motors::stop();
            Navigator::stopped();

            displayProgress( currentHeading );
            Beep::chirp();
            CarrtCallback::yieldMilliseconds( 3000 );

            doFinishedRotating();
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        int currentHeading = roundToInt( LSM303DLHC::getHeading() );
        displayProgress( currentHeading );
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();
        Navigator::stopped();
        MainProcess::changeState( new GotoDriveMenuState );
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
    Display::setCursor( 0, col );
    Display::print( currHeading );
}


void RotateToHeadingState::doFinishedRotating()
{
    // Error: should never get here
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
    // Figure out heading to goal
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    setDesiredHeading( Navigator::convertToCompassAngle( atan2( sGoalY - origY, sGoalX - origX ) ) );
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
};


PerformMappingScanState::PerformMappingScanState()
{
    mHeading = Navigator::getCurrentHeading();
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
   // Every 2 secs....
    if ( event == EventManager::kOneSecondTimerEvent && (param % 2) == 0 )
    {
        if ( mCurrentSlewAngle > kScanLimitRight )
        {
            // Done with scan
            Lidar::slew( 0 );

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
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new GotoDriveMenuState );
    }

    return true;
}


int PerformMappingScanState::getAndProcessRange()
{
    int rng;
    int err = Lidar::getMedianDistanceInCm( &rng );

    if ( !err )
    {
        // Record this observation
        float rad = ( mHeading + mCurrentSlewAngle ) * kDegreesToRadians;
        float x = static_cast<float>( rng ) * cos( rad );
        // Extra negative here because using compass headings instead of mathematical angles
        // math_angle = 360 - compass_angle, which puts a negative on sin()
        float y = -static_cast<float>( rng ) * sin( rad );

        NavigationMap::markObstacle( roundToInt( x + 0.5 ), roundToInt( y + 0.5 ) );
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
        Display::print( -1 );
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
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        Display::clearBottomRow();

        // Do a stage of the process every 1/4 second
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
                MainProcess::changeState( new RotateTowardWaypointState( mTransferX, mTransferY ) );
                break;
        }
    }

    return true;
}


void DetermineNextWaypointState::doGlobalPathStage()
{
    mPath = PathFinder::findPath( mOrigX, mOrigY, sGoalX, sGoalY, NavigationMap::getGlobalMap() );

    if ( mPath->isEmpty() )
    {
        MainProcess::setErrorState( kUnableToFindGlobalPath );
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
        MainProcess::setErrorState( kUnableToFindLocalPath );
    }
}


void DetermineNextWaypointState::doGetLongestDriveStage()
{
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
    // Figure out target heading
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    setDesiredHeading( Navigator::convertToCompassAngle( atan2( wayPointY - origY, wayPointX - origX ) ) );
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
    // Figure out distance/time to waypoint
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    float distanceToDrive = sqrt( (wayPointX - origX)*(wayPointX - origX) + (wayPointY - origY)*(wayPointY - origY) );
    mQtrSecondsToDrive = roundToInt( 4 * DriveParam::timeSecAtFullSpeedGivenDistance( distanceToDrive ) );
}


void DriveToWaypointState::onEntry()
{
    mDriving = false;
    mElapsedQtrSeconds = 0;

    if ( Sonar::getSinglePingDistanceInCm() < kMinDistToObstacle )
    {
        // Somehow there is something immediately in front of us -- shouldn't be there, not good
        // Don't attempt recovery because this really shouldn't happen

        Beep::collisionChime();
        MainProcess::setErrorState( kUnexpectedObstacle );
    }
    else
    {
        Display::clear();
        Display::displayTopRowP16( sDriveTo );

        int secs = ( mElapsedQtrSeconds - 1 ) * 4;
        int dist = static_cast<uint8_t>( DriveParam::distCmAtFullSpeedGivenTime( secs ) + 0.5 );

        uint8_t col = ( dist >= 1000 ? 9 : ( dist >= 100 ? 10 : ( dist >= 10 ? 11 : 12 ) ) );
        Display::setCursor( 0, col );
        Display::print( dist );

        displayDistance();

        // Don't start driving until the first quarter second event
        // Do this to ensure an accurate count
    }
}


void DriveToWaypointState::onExit()
{
    Motors::stop();

    delete this;
}


bool DriveToWaypointState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        ++mElapsedQtrSeconds;

        if ( mElapsedQtrSeconds > mQtrSecondsToDrive )
        {
            // Drive done
            Motors::stop();
            Navigator::stopped();

            gotoNextState();
        }

        if ( !mDriving )
        {
            // Start driving
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
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();
        Navigator::stopped();
        MainProcess::changeState( new GotoDriveMenuState );
    }

    return true;
}


void DriveToWaypointState::gotoNextState()
{
    Vector2Float currentPosition = Navigator::getCurrentPositionCm();
    int origX = roundToInt( currentPosition.x );
    int origY = roundToInt( currentPosition.y );
    float distanceToGoal = sqrt( (sGoalX - origX)*(sGoalX - origX) + (sGoalY - origY)*(sGoalY - origY) );

    if ( fabs( distanceToGoal ) < kCriteriaForGoal )
    {
        MainProcess::changeState( new FinishedGotoDriveState );
    }
    else
    {
        MainProcess::changeState( new FinishedWaypointDriveState( mWayPointX, mWayPointY ) );
    }
}


void DriveToWaypointState::displayDistance()
{
    Display::displayTopRowP16( sDriveSoFar );

    int secs = ( mElapsedQtrSeconds - 1 ) * 4;
    int dist = static_cast<uint8_t>( DriveParam::distCmAtFullSpeedGivenTime( secs ) + 0.5 );

    uint8_t col = ( dist >= 1000 ? 9 : ( dist >= 100 ? 10 : ( dist >= 10 ? 11 : 12 ) ) );
    Display::setCursor( 0, col );
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
    if ( event == EventManager::kOneSecondTimerEvent )
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
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new GotoDriveMenuState );
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
    if ( event == EventManager::kOneSecondTimerEvent )
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
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new GotoDriveMenuState );
    }

    return true;
}








#endif // CARRT_INCLUDE_GOTODRIVE_IN_BUILD



