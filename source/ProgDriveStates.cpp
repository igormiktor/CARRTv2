/*
    ProgDriveStates.cpp - Programmed Drive States for CARRT

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




#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD


#include "ProgDriveStates.h"

#include <stdlib.h>

#include <avr/pgmspace.h>

#include "AVRTools/SystemClock.h"

#include "CarrtCallback.h"
#include "ErrorCodes.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "ProgDriveMenuStates.h"

#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/DriveParam.h"
#include "Drivers/Keypad.h"
#include "Drivers/L3GD20.h"
#include "Drivers/Lidar.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/Motors.h"
#include "Drivers/Sonar.h"




namespace
{
    // Various utility states private to this module


    // Utility state used to ask for confirmation

    class HitAnyKeyState : public State
    {
    public:

        explicit HitAnyKeyState( PGM_P title );

        virtual void onEntry();
        virtual bool onEvent( uint8_t event, int16_t param );

    private:

        PGM_P   mTitle;
    };



    //                                         1234567890123456
    const PROGMEM char sLabelHitAny[]       = "Hit any btn...";


    HitAnyKeyState::HitAnyKeyState( PGM_P title ) :
    mTitle( title )
    {
        // Nothing else
    }


    void HitAnyKeyState::onEntry()
    {
        Display::clear();
        Display::displayTopRowP16( mTitle );
        Display::displayBottomRowP16( sLabelHitAny );
    }


    bool HitAnyKeyState::onEvent( uint8_t event, int16_t param )
    {
        if ( event == EventManager::kKeypadButtonHitEvent )
        {
            MainProcess::changeState( new ProgDriveProgramMenuState );
        }
        return true;
    }



    //**********************************************


    // Special state used to end a program

    //                                         1234567890123456
    const PROGMEM char sLabelFinished[]     = "Prog Drive Done";


    class PgmDrvFinishedState : public HitAnyKeyState
    {
    public:
        PgmDrvFinishedState() : HitAnyKeyState( sLabelFinished ) {}
    };



    //**********************************************


    // Special state used when we hit an obstacle

    //                                         1234567890123456
    const PROGMEM char sLabelObstacle[]     = "Stop! Obstacle!";


    class PgmDrvObstacleState : public HitAnyKeyState
    {
    public:
        PgmDrvObstacleState() : HitAnyKeyState( sLabelObstacle ) {}
    };

};








BaseProgDriveState::BaseProgDriveState() :
mNextStateInProgram( 0 )
{
    // Nothing else to do
}



// onExit() - tear-down the state, but in this class NEVER delete the case on Exit
void BaseProgDriveState::onExit()
{
    // Override State::onEntry() so we NEVER delete ourselves here
    // Explicitly do nothing
}


// Set the next State in the program
BaseProgDriveState* BaseProgDriveState::setNextActionInProgram( BaseProgDriveState* next )
{
    BaseProgDriveState* old = mNextStateInProgram;
    mNextStateInProgram = next;
    return old;
}


void BaseProgDriveState::gotoNextActionInProgram()
{
    if ( mNextStateInProgram )
    {
        MainProcess::changeState( mNextStateInProgram );
    }
    else
    {
        MainProcess::changeState( new PgmDrvFinishedState );
    }
}








//****************************************************************************************


namespace
{
    //                                     1234567890123456
    const PROGMEM char sLabelFwd[]      = "Forward for";
    const PROGMEM char sLabelRev[]      = "Backward for";
    const PROGMEM char sLabelRotL[]     = "Rotate L for";
    const PROGMEM char sLabelRotR[]     = "Rotate R for";
    const PROGMEM char sLabelSecs[]     = "Secs";
};


PgmDrvDriveTimeState::PgmDrvDriveTimeState( Direction dir, uint8_t howManySecondsToDrive ) :
mDirection( dir ),
mQtrSecondsToDrive( 4 * howManySecondsToDrive ),
mElapsedQtrSeconds( 0 ),
mDriving( false )
{
    // Nothing else to do
}


void PgmDrvDriveTimeState::onEntry()
{
    // Don't start driving until first quarter second event
    Motors::stop();
    Motors::setSpeedAllMotors( Motors::kFullSpeed );

    mElapsedQtrSeconds = 0;
    mDriving = false;
    Lidar::slew( 0 );

    Display::clear();
    PGM_P title = 0;
    switch ( mDirection )
    {
        case kForward:
            title = sLabelFwd;
            break;

        case kReverse:
            title = sLabelRev;
            break;

        case kRotateLeft:
            title = sLabelRotL;
            break;

        case kRotateRight:
            title = sLabelRotR;
            break;
    }
    Display::displayTopRowP16( title );

    displaySeconds();
}


void PgmDrvDriveTimeState::displaySeconds()
{
    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::print( (mQtrSecondsToDrive - mElapsedQtrSeconds) / 4 );
    Display::setCursor( 1, 7 );
    Display::printP16( sLabelSecs );
}


void PgmDrvDriveTimeState::onExit()
{
    Motors::stop();
}


bool PgmDrvDriveTimeState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        ++mElapsedQtrSeconds;

        if ( mElapsedQtrSeconds > mQtrSecondsToDrive )
        {
            // Drive done
            Motors::stop();

            gotoNextActionInProgram();
        }

        if ( !mDriving )
        {
            // Start driving
            switch ( mDirection )
            {
                case kForward:
                    Motors::goForward();
                    break;

                case kReverse:
                    Motors::goBackward();
                    break;

                case kRotateLeft:
                    Motors::rotateLeft();
                    break;

                case kRotateRight:
                    Motors::rotateRight();
                    break;
            }

            mDriving = true;
        }

        // If going forward, check for obstacles
        if ( mDirection == kForward )
        {
            // CARRT moves at ~ 35 cm/s

            const int kMinDistToObstacle = 25;   // cm

            if ( Sonar::getSinglePingDistanceInCm() < kMinDistToObstacle )
            {
                // Emergency stop
                Motors::stop();

                MainProcess::changeState( new PgmDrvObstacleState );
            }
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        displaySeconds();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();

        MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}







//****************************************************************************


PgmDrvPauseState::PgmDrvPauseState( uint8_t howManySecondsToPause ) :
mQtrSecondsToPause( 4 * howManySecondsToPause ),
mElapsedQtrSeconds( 0 )
{
    // Nothing else to do
}


void PgmDrvPauseState::onEntry()
{
    mElapsedQtrSeconds = 0;

    Display::clear();
    Display::displayTopRowP16( PSTR( "Pausing for..." ) );

    displaySeconds();
}


bool PgmDrvPauseState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        ++mElapsedQtrSeconds;

        if ( mElapsedQtrSeconds > mQtrSecondsToPause )
        {
            // Pause done
            gotoNextActionInProgram();
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        displaySeconds();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
       MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}


void PgmDrvPauseState::displaySeconds()
{
    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::print( (mQtrSecondsToPause - mElapsedQtrSeconds) / 4 );
    Display::setCursor( 1, 7 );
    Display::printP16( sLabelSecs );
}







//****************************************************************************


PgmDrvBeepState::PgmDrvBeepState( uint8_t howManySecondsToBeep ) :
mQtrSecondsToBeep( 4 * howManySecondsToBeep ),
mElapsedQtrSeconds( 0 )
{
    // Nothing else to do
}


void PgmDrvBeepState::onEntry()
{
    mElapsedQtrSeconds = 0;

    Display::clear();
    Display::displayTopRowP16( PSTR( "Beeping for..." ) );

    displaySeconds();

    Beep::beepOn();
}


void PgmDrvBeepState::onExit()
{
    Beep::beepOff();
}


bool PgmDrvBeepState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        ++mElapsedQtrSeconds;

        if ( mElapsedQtrSeconds > mQtrSecondsToBeep )
        {
            // Beep done
            gotoNextActionInProgram();
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        displaySeconds();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}


void PgmDrvBeepState::displaySeconds()
{
    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::print( (mQtrSecondsToBeep - mElapsedQtrSeconds) / 4 );
    Display::setCursor( 1, 7 );
    Display::printP16( sLabelSecs );
}







//****************************************************************************


namespace
{
    const int8_t kScanLimitLeft         = -70;
    const int8_t kScanLimitRight        = 70;
    const int8_t kScanIncrement         = 5;

    const PROGMEM char sLabelScaning[]  = "Scanning...";
    const PROGMEM char sLabelRng[]      = "Rng = ";
    const PROGMEM char sLabelAngle[]    = "Angle = ";
};


void PgmDrvScanState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sLabelScaning );

    mCurrentSlewAngle = kScanLimitLeft;
    Lidar::slew( mCurrentSlewAngle );

    // Allow time for the servo to slew (this might be a big slew)
    CarrtCallback::yieldMilliseconds( 500 );

    displayAngleRange();
}


void PgmDrvScanState::onExit()
{
    Lidar::slew( 0 );
}


bool PgmDrvScanState::onEvent( uint8_t event, int16_t param )
{
    const int kSlewTimePause = 250;     // Time for servo to slew in msec
    
    // Every 2 secs....
    if ( event == EventManager::kOneSecondTimerEvent && (param % 2) == 0 )
    {
        mCurrentSlewAngle += kScanIncrement;
        if ( mCurrentSlewAngle > kScanLimitRight )
        {
            // Done with Scan
            Lidar::slew( 0 );
            gotoNextActionInProgram();
        }
        else
        {
            // Slew radar into position for next read
            Lidar::slew( mCurrentSlewAngle );

            // Allow time for the servo to slew (this is a small slew)
            CarrtCallback::yieldMilliseconds( kSlewTimePause );

            displayAngleRange();
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}


void PgmDrvScanState::displayAngleRange()
{
    int rng;
    int err = Lidar::getDistanceInCm( &rng );

    Display::clear();
    Display::setCursor( 0, 0 );
    Display::printP16( sLabelAngle );
    Display::setCursor( 0, 8 );
    Display::print( mCurrentSlewAngle );
    Display::setCursor( 1, 0 );
    Display::printP16( sLabelRng );
    Display::setCursor( 1, 6 );
    if ( err )
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
    //                                         1234567890123456
    const PROGMEM char sLabelRot[]          = "Rotate";
    const PROGMEM char sTgt[]               = "Tgt";
    const PROGMEM char sCur[]               = "Now";
};


// cppcheck-suppress uninitMemberVar
PgmDrvRotAngleState::PgmDrvRotAngleState( int rotationAngle ) :
mRotationAngle( rotationAngle )
{
    // Nothing else
}


void PgmDrvRotAngleState::onEntry()
{
    mPriorLeftToGo = 360;

    mGoLeft = ( mRotationAngle > 0 );

    mTargetHeading = static_cast<int>( LSM303DLHC::getHeading() ) - mRotationAngle + 360;
    mTargetHeading %= 360;

    Display::clear();
    Display::setCursor( 0, 0 );
    Display::printP16( sLabelRot );
    Display::setCursor( 0, 11 );
    Display::print( mRotationAngle );

    displayProgress( static_cast<int>( LSM303DLHC::getHeading() ) );

    Motors::setSpeedAllMotors( Motors::kFullSpeed );

    if ( mGoLeft )
    {
        Motors::rotateLeft();
    }
    else
    {
        Motors::rotateRight();
    }
}


void PgmDrvRotAngleState::onExit()
{
    Motors::stop();
    Motors::setSpeedAllMotors( Motors::kFullSpeed );
}


bool PgmDrvRotAngleState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        int currentHeading = static_cast<int>( LSM303DLHC::getHeading() );

        if ( rotationDone( currentHeading ) )
        {
            Motors::stop();

            displayProgress( currentHeading );
            CarrtCallback::yieldMilliseconds( 3000 );

            gotoNextActionInProgram();
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        int currentHeading = static_cast<int>( LSM303DLHC::getHeading() );
        displayProgress( currentHeading );
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
       MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}


bool PgmDrvRotAngleState::rotationDone( int currHeading )
{
    const int kSlowThreshold = 25;
    const int kHeadingThreshold = 5;        // degrees

    int delta = mTargetHeading - currHeading;
    int deltaAbs = abs( delta );
    if ( deltaAbs > 180 )
    {
        deltaAbs = 360 - deltaAbs;
    }

    if ( deltaAbs < kHeadingThreshold )
    {
        return true;
    }

    if ( deltaAbs < kSlowThreshold )
    {
        // We are close, so slow down
        Motors::setSpeedAllMotors( Motors::kHalfSpeed );
    }

    if ( deltaAbs <= mPriorLeftToGo )
    {
        mPriorLeftToGo = deltaAbs;
    }
    else
    {
        // delta got bigger: passed the target heading, so reverse
        reverseDirection();
    }

    return false;
}


void PgmDrvRotAngleState::reverseDirection()
{
    Motors::stop();
    if ( mGoLeft )
    {
        Motors::rotateRight();
        mGoLeft = false;
    }
    else
    {
        Motors::rotateLeft();
        mGoLeft = true;
    }
    // Reset left to go...
    mPriorLeftToGo = 360;
}


void PgmDrvRotAngleState::displayProgress( int currHeading )
{
    //  0123456789012345
    //  Tgt xxx Now xxx

    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::printP16( sTgt );
    Display::setCursor( 1, 4 );
    Display::print( mTargetHeading );
    Display::setCursor( 1, 8 );
    Display::printP16( sCur );
    Display::setCursor( 1, 12 );
    Display::print( currHeading );
}







//****************************************************************************


namespace
{
    //                                         1234567890123456
    const PROGMEM char sLabelFwdD[]         = "Forward";
    const PROGMEM char sLabelRevD[]         = "Reverse";
    const PROGMEM char sLabelCm[]           = "cm";
    const PROGMEM char sLabelSoFar[]        = "So far";
};


// cppcheck-suppress uninitMemberVar
PgmDrvDriveDistanceState::PgmDrvDriveDistanceState( uint8_t direction, uint8_t distInCm ) :
mDistanceCm( distInCm ),
mGoForward( direction == kForward )
{
    // Nothing else
}


void PgmDrvDriveDistanceState::onEntry()
{
    float secsToDrive = DriveParam::timeSecAtFullSpeedGivenDistanceCm( mDistanceCm );
    mQtrSecondsToDrive = static_cast<uint8_t>( secsToDrive * 4 + 0.5 );

    mDriving = false;
    mElapsedQtrSeconds = 0;

    Display::clear();
    if ( mGoForward )
    {
        Display::displayTopRowP16( sLabelFwdD );
    }
    else
    {
        Display::displayTopRowP16( sLabelRevD );
    }

    Display::setCursor( 0, 9 );
    Display::print( mDistanceCm );
    Display::setCursor( 0, 14 );
    Display::printP16( sLabelCm );

    displayDistance();

    Motors::setSpeedAllMotors( Motors::kFullSpeed );
}


void PgmDrvDriveDistanceState::onExit()
{
    Motors::stop();
}


bool PgmDrvDriveDistanceState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        ++mElapsedQtrSeconds;

        if ( mElapsedQtrSeconds > mQtrSecondsToDrive )
        {
            // Drive done
            Motors::stop();

            gotoNextActionInProgram();
        }

        if ( !mDriving )
        {
            // Start driving
            if ( mGoForward )
            {
                Motors::goForward();
            }
            else
            {
                Motors::goBackward();
            }

            mDriving = true;
        }

        // If going forward, check for obstacles
        if ( mGoForward )
        {
            // CARRT moves at ~ 39 cm/s

            const int kMinDistToObstacle = 25;   // cm

            if ( Sonar::getSinglePingDistanceInCm() < kMinDistToObstacle )
            {
                // Emergency stop
                Motors::stop();

                MainProcess::changeState( new PgmDrvObstacleState );
            }
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        displayDistance();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();

        MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}


void PgmDrvDriveDistanceState::displayDistance()
{
    Display::clearBottomRow();
    Display::printP16( sLabelSoFar );
    Display::setCursor( 1, 9 );
    int dist = static_cast<uint8_t>( DriveParam::distCmAtFullSpeedGivenQuarterSeconds( mElapsedQtrSeconds - 1 ) + 0.5 );
    Display::print( dist );
    Display::setCursor( 1, 14 );
    Display::printP16( sLabelCm );
}













#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD
