/*
    ProgDriveStates.cpp - Programmed Drive States for CARRT

    Copyright (c) 2016 Igor Mikolic-Torreira.  All right reserved.

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

#include <avr/pgmspace.h>

#include "AVRTools/SystemClock.h"

#include "CarrtCallback.h"
#include "ErrorCodes.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "ProgDriveMenuStates.h"

#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/Keypad.h"
#include "Drivers/L3GD20.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/Motors.h"
#include "Drivers/Radar.h"




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



// onExit() - tear-down the state, but in this classe NEVER delete the case on Exit
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

    mElapsedQtrSeconds = 0;
    mDriving = false;
    Radar::slew( 0 );

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
    Display::print( sLabelSecs );
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
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        displaySeconds();

        if ( mDirection == kForward )
        {
            const int kMinDistToObstacle = 25;   // cm

            // if moving forward, every second check for obstacles
            if ( Radar::getDistanceInCm() < kMinDistToObstacle )
            {
                // Emergency stop
                Motors::stop();

                MainProcess::changeState( new PgmDrvObstacleState );
            }
        }
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
    Motors::stop();

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
    Display::print( sLabelSecs );
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
    Motors::stop();

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
    Display::print( sLabelSecs );
}







//****************************************************************************


namespace
{
    const int8_t kScanLimitLeft         = -60;
    const int8_t kScanLimitRight        = 60;
    const int8_t kScanIncrement         = 10;

    const PROGMEM char sLabelScaning[]  = "Scanning...";
    const PROGMEM char sLabelRng[]      = "Rng = ";
    const PROGMEM char sLabelAngle[]    = "Angle = ";
};


void PgmDrvScanState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( sLabelScaning );

    mCurrentSlewAngle = kScanLimitLeft;
    Radar::slew( mCurrentSlewAngle );

    // Allow time for the servo to slew (this might be a big slew)
    CarrtCallback::yield( 500 );
}


void PgmDrvScanState::onExit()
{
    Radar::slew( 0 );
}


bool PgmDrvScanState::onEvent( uint8_t event, int16_t param )
{
    // Every 4 secs....
    if ( event == EventManager::kEightSecondTimerEvent && (param % 4) == 0 )
    {
        // Read the current position
        displayAngleRange();

        mCurrentSlewAngle += kScanIncrement;
        if ( mCurrentSlewAngle > kScanLimitRight )
        {
            // Done with Scan
            gotoNextActionInProgram();
        }

        // Slew radar into position for next read
        Radar::slew( mCurrentSlewAngle );
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}


void PgmDrvScanState::displayAngleRange()
{
    int rng = Radar::getDistanceInCm();

    Display::clear();
    Display::setCursor( 0, 0 );
    Display::printP16( sLabelAngle );
    Display::setCursor( 0, 8 );
    Display::print( mCurrentSlewAngle );
    Display::setCursor( 1, 0 );
    Display::printP16( sLabelRng );
    Display::setCursor( 1, 6 );
    Display::print( rng );
}














#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD
