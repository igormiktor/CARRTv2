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

#include "CarrtCallback.h"
#include "DriveProgram.h"
#include "ErrorCodes.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "ProgDriveStates.h"
#include "WelcomeMenuStates.h"

#include "Drivers/Display.h"
#include "Drivers/Keypad.h"





namespace
{

    // Drive Program menu-related classes internal to this module




    //********************************************************************


    class ProgDriveAnyTimeMenuState : public State
    {
    public:

        enum Action
        {
            kForward,
            kReverse,
            kRotateLeft,
            kRotateRight,
            kPause,
            kBeep
        };

        explicit ProgDriveAnyTimeMenuState( Action action );

        virtual void onEntry();
        virtual void onExit();
        virtual bool onEvent( uint8_t event, int16_t param );

    private:

        void displaySecondsSetting();

        const Action    mAction;
        uint8_t         mSeconds;
    };




    //********************************************************************


    class ProgDriveFwdTimeMenuState : public ProgDriveAnyTimeMenuState
    {
    public:

        ProgDriveFwdTimeMenuState() : ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kForward ) {}
    };



    class ProgDriveRevTimeMenuState : public ProgDriveAnyTimeMenuState
    {
    public:

        ProgDriveRevTimeMenuState() : ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kReverse ) {}
    };



    class ProgDriveRotLTimeMenuState : public ProgDriveAnyTimeMenuState
    {
    public:

        ProgDriveRotLTimeMenuState() : ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kRotateLeft ) {}
    };



    class ProgDriveRotRTimeMenuState : public ProgDriveAnyTimeMenuState
    {
    public:

        ProgDriveRotRTimeMenuState() : ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kRotateRight ) {}
    };



    class ProgDrivePauseTimeMenuState : public ProgDriveAnyTimeMenuState
    {
    public:

        ProgDrivePauseTimeMenuState() : ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kPause ) {}
    };



    class ProgDriveBeepTimeMenuState : public ProgDriveAnyTimeMenuState
    {
    public:

        ProgDriveBeepTimeMenuState() : ProgDriveAnyTimeMenuState( ProgDriveAnyTimeMenuState::kBeep ) {}
    };










    //********************************************************************


    class ProgDriveSelectIntMenuState : public State
    {
    public:

        ProgDriveSelectIntMenuState( PGM_P title, int min, int max, int inc, int initial );

        virtual void onEntry();
        virtual bool onEvent( uint8_t event, int16_t param );

        virtual State* onSelection( int value ) = 0;

    private:

        void displayValue();

        PGM_P   mTitle;
        int     mMin;
        int     mMax;
        int     mInc;
        int     mValue;
    };






    //********************************************************************


    class ProgDriveRotateAngleMenuState : public ProgDriveSelectIntMenuState
    {
    public:

        ProgDriveRotateAngleMenuState();

        virtual State* onSelection( int value );
    };






    //********************************************************************


    class ProgDriveForwardDistanceMenuState : public ProgDriveSelectIntMenuState
    {
    public:

        ProgDriveForwardDistanceMenuState();

        virtual State* onSelection( int value );
    };






    //********************************************************************


    class ProgDriveReverseDistanceMenuState : public ProgDriveSelectIntMenuState
    {
    public:

        ProgDriveReverseDistanceMenuState();

        virtual State* onSelection( int value );
    };









    //********************************************************************


    class ProgDriveYesNoState : public State
    {
    public:

        explicit ProgDriveYesNoState( PGM_P title );

        virtual void onEntry();
        virtual bool onEvent( uint8_t event, int16_t param );

        virtual State* onYes() = 0;
        virtual State* onNo() = 0;

    private:

        void displayYesNo();

        PGM_P   mTitle;
        bool    mYes;
    };






    //********************************************************************


    class ProgDriveClearState : public ProgDriveYesNoState
    {
    public:

        ProgDriveClearState();

        virtual State* onYes();
        virtual State* onNo();
    };



};





namespace
{
    //                                             1234567890123456
    const PROGMEM char sPgmDrvProgMenuTitle[]   = "Add Action";
    const PROGMEM char sPgmDrvProgMenuItem00[]  = "Exit...";
    const PROGMEM char sPgmDrvProgMenuItem01[]  = "Go Fwd Time";
    const PROGMEM char sPgmDrvProgMenuItem02[]  = "Go Fwd Dist";
    const PROGMEM char sPgmDrvProgMenuItem03[]  = "Go Rev Time";
    const PROGMEM char sPgmDrvProgMenuItem04[]  = "Go Rev Dist";
    const PROGMEM char sPgmDrvProgMenuItem05[]  = "Rotate L Time";
    const PROGMEM char sPgmDrvProgMenuItem06[]  = "Rotate R Time";
    const PROGMEM char sPgmDrvProgMenuItem07[]  = "Rotate Angle";
    const PROGMEM char sPgmDrvProgMenuItem08[]  = "Pause";
    const PROGMEM char sPgmDrvProgMenuItem09[]  = "Beep";
    const PROGMEM char sPgmDrvProgMenuItem10[]  = "Scan";
    const PROGMEM char sPgmDrvProgMenuItem11[]  = "Run it...";
    const PROGMEM char sPgmDrvProgMenuItem12[]  = "Clear...";

    const PROGMEM char sPgmDrvProgMenuScan[]    = "Added Scan...";
    const PROGMEM char sPgmDrvProgMenuGo[]      = "Running in...";


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
        { sPgmDrvProgMenuItem12,  12 },

        { sPgmDrvProgMenuItem00,  0 }
    };



    State* addScanAction()
    {
        Display::clear();
        Display::displayTopRowP16( sPgmDrvProgMenuScan );
        DriveProgram::addAction( new PgmDrvScanState );
        CarrtCallback::yield( 500 );
        return new ProgDriveProgramMenuState;
    }



    State* prepFirstActionInProgram()
    {
        // Add a little pause before program runs...
        Display::displayTopRowP16( sPgmDrvProgMenuGo );
        Display::clearBottomRow();
        for ( int8_t n = 3; n >= 0; --n )
        {
            Display::setCursor( 1, 0 );
            Display::print( n );
            CarrtCallback::yield( 500 );
        }

        State* progStartState = DriveProgram::getProgramStart();
        if ( !progStartState )
        {
            MainProcess::postErrorEvent( kNullStateInProgram );
            progStartState = new ProgDriveProgramMenuState;
        }
        return progStartState;
    }



    State* getPgmDrvProgMenuState( uint8_t menuId )
    {
        switch ( menuId )
        {
            case 0:
                return new WelcomeState;

            case 1:
                return new ProgDriveFwdTimeMenuState;

            case 2:
                return new ProgDriveForwardDistanceMenuState;

            case 3:
                return new ProgDriveRevTimeMenuState;

            case 4:
                return new ProgDriveReverseDistanceMenuState;

            case 5:
                return new ProgDriveRotLTimeMenuState;

            case 6:
                return new ProgDriveRotRTimeMenuState;

            case 7:
                return new ProgDriveRotateAngleMenuState;

            case 8:
                return new ProgDrivePauseTimeMenuState;

            case 9:
                return new ProgDriveBeepTimeMenuState;

            case 10:
                return addScanAction();

            case 11:
                return prepFirstActionInProgram();

            case 12:
                return new ProgDriveClearState;

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


void ProgDriveProgramMenuState::onEntry()
{
    MenuState::onEntry();

    uint8_t len = DriveProgram::len();

    Display::setCursor( 0, 11 );
    Display::print( '(' );
    uint8_t pos = 12;
    if ( len < 100 )
    {
        pos = 13;
    }
    if ( len < 10 )
    {
        pos = 14;
    }
    Display::setCursor( 0, pos );
    Display::print( len );
    Display::setCursor( 0, 15 );
    Display::print( ')' );
}










//********************************************************************


namespace
{
    const PROGMEM char sLabelYes[]  = "Yes";
    const PROGMEM char sLabelNo[]   = "No";
};


ProgDriveYesNoState::ProgDriveYesNoState( PGM_P title ) :
mTitle( title ),
mYes( 0 )
{
    // Nothing else
}


void ProgDriveYesNoState::onEntry()
{
    mYes = 0;
    Display::clear();
    Display::displayTopRowP16( mTitle );
    displayYesNo();
}


bool ProgDriveYesNoState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            // Got our answer
            State* newState;
            if ( mYes )
            {
                newState = onYes();
            }
            else
            {
                newState = onNo();
            }
            MainProcess::changeState( newState );
        }
        else
        {
            // Toggle the answer
            mYes = !mYes;
            displayYesNo();
        }
    }
    return true;
}


void ProgDriveYesNoState::displayYesNo()
{
    if ( mYes )
    {
        Display::displayBottomRowP16( sLabelYes );
    }
    else
    {
        Display::displayBottomRowP16( sLabelNo );
    }
}











//********************************************************************


ProgDriveSelectIntMenuState::ProgDriveSelectIntMenuState( PGM_P title, int min, int max, int inc, int initial ) :
mTitle( title ),
mMin( min ),
mMax( max ),
mInc( inc ),
mValue( initial )
{
    // Nothing else
}


void ProgDriveSelectIntMenuState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( mTitle );
    displayValue();
}


bool ProgDriveSelectIntMenuState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            // Got our answer
            State* newState = onSelection( mValue );
            MainProcess::changeState( newState );
        }
        else if ( button & Keypad::kButton_Left || button & Keypad::kButton_Down )
        {
            // Decrement value
            mValue -= mInc;

            if ( mValue < mMin )
            {
                mValue = mMin;
            }

            displayValue();
        }
        else if ( button & Keypad::kButton_Right || button & Keypad::kButton_Up )
        {
            // Increment value
            mValue += mInc;

            if ( mValue > mMax )
            {
                mValue = mMax;
            }

            displayValue();
        }
    }
    return true;
}


void ProgDriveSelectIntMenuState::displayValue()
{
    Display::clearBottomRow();
    Display::setCursor( 1, 3 );
    Display::print( mValue );
}











//******************************************************************************


namespace
{
    //                                         1234567890123456
    const PROGMEM char sLabelClearPgm[]     = "Clear Drive Pgm?";
};


ProgDriveClearState::ProgDriveClearState() :
ProgDriveYesNoState( sLabelClearPgm )
{
    // Nothing else
}


State* ProgDriveClearState::onYes()
{
    // Delete the current drive program
    DriveProgram::purge();
    return new ProgDriveProgramMenuState;
}


State* ProgDriveClearState::onNo()
{
   return new ProgDriveProgramMenuState;
}








//******************************************************************************


namespace
{
    //                                              1234567890123456
    const PROGMEM char sLabelRotAngleTitle[]     = "Degs to Rotate?";
};


ProgDriveRotateAngleMenuState::ProgDriveRotateAngleMenuState() :
ProgDriveSelectIntMenuState( sLabelRotAngleTitle, -180, 180, 10, 0 )
{
    // Nothing else
}


State* ProgDriveRotateAngleMenuState::onSelection( int value )
{
    DriveProgram::addAction( new PgmDrvRotAngleState( value ) );

    return new ProgDriveProgramMenuState;
}








//******************************************************************************


namespace
{
    //                                                 1234567890123456
    const PROGMEM char sLabelFwdDistanceTitle[]  = "CMs to go Fwd?";
};


ProgDriveForwardDistanceMenuState::ProgDriveForwardDistanceMenuState() :
ProgDriveSelectIntMenuState( sLabelFwdDistanceTitle, 0, 500, 20, 0 )
{
    // Nothing else
}


State* ProgDriveForwardDistanceMenuState::onSelection( int value )
{
    DriveProgram::addAction( new PgmDrvDriveDistanceState( PgmDrvDriveDistanceState::kForward, value ) );

    return new ProgDriveProgramMenuState;
}








//******************************************************************************


namespace
{
    //                                              1234567890123456
    const PROGMEM char sLabelRevDistanceTitle[]  = "CMs to go Rev?";
};


ProgDriveReverseDistanceMenuState::ProgDriveReverseDistanceMenuState() :
ProgDriveSelectIntMenuState( sLabelRevDistanceTitle, 0, 500, 20, 0 )
{
    // Nothing else
}


State* ProgDriveReverseDistanceMenuState::onSelection( int value )
{
    DriveProgram::addAction( new PgmDrvDriveDistanceState( PgmDrvDriveDistanceState::kReverse, value ) );

    return new ProgDriveProgramMenuState;
}








//******************************************************************************


namespace
{
    //                                         1234567890123456
    const PROGMEM char sLabelFwdTime[]      = "Fwd how long?";
    const PROGMEM char sLabelRevTime[]      = "Rev how long?";
    const PROGMEM char sLabelRotLTime[]     = "Rot L how long?";
    const PROGMEM char sLabelRotRTime[]     = "Rot R how long?";
    const PROGMEM char sLabelPauseTime[]    = "Pause how long?";
    const PROGMEM char sLabelBeepTime[]     = "Beep how long?";

    const PROGMEM char sLabelSecs[]         = "secs";
};



ProgDriveAnyTimeMenuState::ProgDriveAnyTimeMenuState( Action action ) :
mAction( action )
{
}


void ProgDriveAnyTimeMenuState::onEntry()
{
    mSeconds = 1;

    PGM_P topLabel = 0;
    switch ( mAction )
    {
        case kForward:
            topLabel = sLabelFwdTime;
            break;

        case kReverse:
            topLabel = sLabelRevTime;
            break;

        case kRotateLeft:
            topLabel = sLabelRotLTime;
            break;

        case kRotateRight:
            topLabel = sLabelRotRTime;
            break;

        case kPause:
            topLabel = sLabelPauseTime;
            break;

        case kBeep:
            topLabel = sLabelBeepTime;
            break;
    }

    Display::clear();
    Display::displayTopRowP16( topLabel );

    displaySecondsSetting();
}


void ProgDriveAnyTimeMenuState::onExit()
{
    // Create the appropriate action
    BaseProgDriveState* newAction = 0;
    switch ( mAction )
    {
        case kForward:
            newAction = new PgmDrvDriveTimeState( PgmDrvDriveTimeState::kForward, mSeconds );
            break;

        case kReverse:
            newAction = new PgmDrvDriveTimeState( PgmDrvDriveTimeState::kReverse, mSeconds );
            break;

        case kRotateLeft:
            newAction = new PgmDrvDriveTimeState( PgmDrvDriveTimeState::kRotateLeft, mSeconds );
            break;

        case kRotateRight:
            newAction = new PgmDrvDriveTimeState( PgmDrvDriveTimeState::kRotateRight, mSeconds );
            break;

        case kPause:
            newAction = new PgmDrvPauseState( mSeconds );
            break;

        case kBeep:
            newAction = new PgmDrvBeepState( mSeconds );
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
    Display::setCursor( 1, 0 );
    Display::print( mSeconds );
    Display::setCursor( 1, 12 );
    Display::printP16( sLabelSecs );
};






#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD



