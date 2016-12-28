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
//#include "AVRTools/MemUtils.h"

#include "CarrtCallback.h"
// #include "CarrtPins.h"
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

        HitAnyKeyState( PGM_P title );

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
    const PROGMEM char sLabelSecs[]     = "Secs";
};


PgmDrvDriveTimeState::PgmDrvDriveTimeState( Direction dir, uint8_t howManySecondsToDrive ) :
mDirection( dir ),
mSecondsToDrive( howManySecondsToDrive ),
mElapsedSeconds( 0 ),
mDriving( false )
{
    // Nothing else to do
}


void PgmDrvDriveTimeState::onEntry()
{
    // Don't start driving until a one second event
    Motors::stop();

    mElapsedSeconds = 0;
    mDriving = false;
    Radar::slew( 0 );

    Display::clear();
    PGM_P title;
    switch ( mDirection )
    {
        case kForward:
            title = sLabelFwd;
            break;

        case kReverse:
            title = sLabelRev;
            break;
    }
    Display::displayTopRowP16( title );
    Display::setCursor( 1, 0 );
    Display::print( mSecondsToDrive );
    Display::setCursor( 1, 7 );
    Display::print( sLabelSecs );
}


void PgmDrvDriveTimeState::onExit()
{
    Motors::stop();
    mElapsedSeconds = 0;
    mDriving = false;
}


bool PgmDrvDriveTimeState::onEvent( uint8_t event, int16_t param )
{
    const int kMinDistToObstacle    = 20;       // cm

    // If driving forward,on every half-second...
    if ( mDriving && mDirection == kForward && event == EventManager::kQuarterSecondTimerEvent && param % 2 )
    {
        // ...check for obstacles
        if ( Radar::getDistanceInCm() < kMinDistToObstacle )
        {
            // Emergency stop
            Motors::stop();

            MainProcess::changeState( new PgmDrvObstacleState );
        }
    }
    else if ( event == EventManager::kOneSecondTimerEvent )
    {
        ++mElapsedSeconds;
        if ( mElapsedSeconds == 1 )
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
            }
        }
        else if ( mElapsedSeconds > mSecondsToDrive )
        {
            // Done driving
            Motors::stop();

            gotoNextActionInProgram();
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();

        MainProcess::changeState( new ProgDriveProgramMenuState );
    }

    return true;
}






#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD
