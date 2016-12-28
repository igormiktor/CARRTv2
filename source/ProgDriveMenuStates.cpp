/*
    ProgDriveMenuStates.cpp - Programmed Drive Menu States for CARRT

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


#include "ProgDriveMenuStates.h"

#include <avr/pgmspace.h>

#include "DriveProgram.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "WelcomeMenuStates.h"

#include "Drivers/Display.h"
#include "Drivers/Keypad.h"





void ProgDriveState::onEntry()
{
    Display::displayTopRowP16( PSTR( "Prog Drive Menu" ) );
    Display::displayBottomRowP16( PSTR( "Forthcoming..." ) );
}


bool ProgDriveState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new WelcomeState );
    }
}





namespace
{
    //                                            1234567890123456
    const PROGMEM char sPgmDrvProgMenuTitle[]  = "Add Action->";
    const PROGMEM char sPgmDrvProgMenuItem00[] = "Abort...";
    const PROGMEM char sPgmDrvProgMenuItem01[] = "Go Fwd Time";
    const PROGMEM char sPgmDrvProgMenuItem02[] = "Go Fwd Dist";
    const PROGMEM char sPgmDrvProgMenuItem03[] = "Go Rev Time";
    const PROGMEM char sPgmDrvProgMenuItem04[] = "Go Rev Dist";
    const PROGMEM char sPgmDrvProgMenuItem05[] = "Rotate L Time";
    const PROGMEM char sPgmDrvProgMenuItem06[] = "Rotate R Time";
    const PROGMEM char sPgmDrvProgMenuItem07[] = "Rotate Angle";
    const PROGMEM char sPgmDrvProgMenuItem08[] = "Pause";
    const PROGMEM char sPgmDrvProgMenuItem09[] = "Beep";
    const PROGMEM char sPgmDrvProgMenuItem10[] = "Scan";
    const PROGMEM char sPgmDrvProgMenuItem11[] = "Done...";


    const PROGMEM MenuList sPgmDrvProgMenu[] =
    {
        { sPgmDrvProgMenuItem01,  1 },
        { sPgmDrvProgMenuItem02,  2 },
        { sPgmDrvProgMenuItem03,  3 },
        { sPgmDrvProgMenuItem04,  4 },
        { sPgmDrvProgMenuItem05,  5 },
        { sPgmDrvProgMenuItem06,  6 },
        { sPgmDrvProgMenuItem07,  7 },
        { sPgmDrvProgMenuItem08,  8 },
        { sPgmDrvProgMenuItem09,  9 },
        { sPgmDrvProgMenuItem10,  10 },
        { sPgmDrvProgMenuItem11,  11 },

        { sPgmDrvProgMenuItem00,  0 }
    };


    State* getPgmDrvProgMenuState( uint8_t menuId )
    {
        switch ( menuId )
        {
            case 0:
                return 0;                                   // TODO replace with correct version

            case 1:
                return new ProgDriveFwdTimeMenuState;

            case 2:
                return 0;                                   // TODO replace with correct version

            case 3:
                return new ProgDriveRevTimeMenuState;

            case 4:
                return 0;                                   // TODO replace with correct version

            case 5:
                return new ProgDriveRotLTimeMenuState;

            case 6:
                return new ProgDriveRotRTimeMenuState;

            case 7:
                return 0;                                   // TODO replace with correct version

            case 8:
                return 0;                                   // TODO replace with correct version

            case 9:
                return 0;                                   // TODO replace with correct version

            case 10:
                return 0;                                   // TODO replace with correct version

            case 11:
                return 0;                                   // TODO replace with correct version

            default:
                return 0;
        }
    }
}




ProgDriveProgramMenuState::ProgDriveProgramMenuState() :
MenuState( sPgmDrvProgMenuTitle, sPgmDrvProgMenu, sizeof( sPgmDrvProgMenu ) / sizeof( MenuItem ), getPgmDrvProgMenuState )
{
    // Nothing else to do
}





//******************************************************************************

//                                                     1234567890123456
const PROGMEM char sPrgDrvMenuLabelFwdTime[]        = "Fwd how long?";
const PROGMEM char sPrgDrvMenuLabelRevTime[]        = "Rev how long?";
const PROGMEM char sPrgDrvMenuLabelRotLTime[]       = "Rot L how long?";
const PROGMEM char sPrgDrvMenuLabelRotRTime[]       = "Rot R how long?";

const PROGMEM char sPrgDrvMenuLabelSecs[]           = "secs";

    enum Action
    {
        kForward,
        kReverse,
        kRotateLeft,
        kRotateRight
    };



ProgDriveAnyTimeMenuState::ProgDriveAnyTimeMenuState( Action action ) :
mAction( action )
{
}


void ProgDriveAnyTimeMenuState::onEntry()
{
    mSeconds = 1;

    PGM_P topLabel;
    switch ( mAction )
    {
        case kForward:
            topLabel = sPrgDrvMenuLabelFwdTime;
            break;

        case kReverse:
            topLabel = sPrgDrvMenuLabelRevTime;
            break;

        case kRotateLeft:
            topLabel = sPrgDrvMenuLabelRotLTime;
            break;

        case kRotateRight:
            topLabel = sPrgDrvMenuLabelRotRTime;
            break;
    }

    Display::clear();
    Display::displayTopRowP16( topLabel );

    displaySecondsSetting();
}


void ProgDriveAnyTimeMenuState::onExit()
{
    // Create the appropriate action
    BaseProgDriveState* newAction;
    switch ( mAction )
    {
        case kForward:
            newAction = new PgmDrvForwardTime( mSeconds );
            break;

        case kReverse:
            newAction = new PgmDrvForwardTime( mSeconds );      // TODO Replace with correct version
            break;

        case kRotateLeft:
            newAction = new PgmDrvForwardTime( mSeconds );      // TODO Replace with correct version
            break;

        case kRotateRight:
            newAction = new PgmDrvForwardTime( mSeconds );      // TODO Replace with correct version
            break;
    }

    // Save the action we have
    DriveProgram::addAction( newAction );

}


bool ProgDriveAnyTimeMenuState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Left || button & Keypad::kButton_Down )
        {
            // Decrease seconds down, but not below one
            if ( mSeconds > 1 )
            {
                --mSeconds;
            }

            displaySecondsSetting();
        }
        else if ( button & Keypad::kButton_Right || button & Keypad::kButton_Up )
        {
            // Increase seconds, but not more than 16 secs
            if ( mSeconds < 16 )
            {
                ++mSeconds;
            }

            displaySecondsSetting();
        }
        else if ( button & Keypad::kButton_Select )
        {
            // Done, go back to main programming menu
            MainProcess::changeState( new ProgDriveProgramMenuState );
        }
    }

    return true;
}


void ProgDriveAnyTimeMenuState::displaySecondsSetting()
{
    Display::clearBottomRow();
    Display::print( mSeconds );
    Display::setCursor( 1, 12 );
    Display::printP16( sPrgDrvMenuLabelSecs );
};






//******************************************************************************


ProgDriveFwdTimeMenuState::ProgDriveFwdTimeMenuState() :
ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kForward )
{
    // Nothing else to do
}





//******************************************************************************


ProgDriveRevTimeMenuState::ProgDriveRevTimeMenuState() :
ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kReverse )
{
    // Nothing else to do
}





//******************************************************************************


ProgDriveRotLTimeMenuState::ProgDriveRotLTimeMenuState() :
ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kRotateLeft )
{
    // Nothing else to do
}





//******************************************************************************


ProgDriveRotRTimeMenuState::ProgDriveRotRTimeMenuState() :
ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kRotateRight )
{
    // Nothing else to do
}












#if 0





ProgDriveRunState::ProgDriveRunState( BaseProgDriveState* program ) :
mProgram( program )
{
    // Nothing else to do
}

void ProgDriveRunState::onEntry();
bool ProgDriveRunState::onEvent( uint8_t event, int16_t param );

private:

    BaseProgDriveState* mProgram;






class ProgDriveInterruptState : public MenuState
{
public:

    explicit ProgDriveInterruptState( BaseProgDriveState* program );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    BaseProgDriveState* mProgram;
};





class ProgDriveTearDownState : public MenuState
{
public:

    explicit ProgDriveTearDownState( BaseProgDriveState* program );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    BaseProgDriveState* mProgram;
};

#endif



#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD



