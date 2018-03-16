/*
    GotoDriveMenuStates.cpp - Goto Drive Menu States for CARRT

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



#include "GotoDriveMenuStates.h"

#include <stdlib.h>

#include <avr/pgmspace.h>

#include "CarrtCallback.h"
#include "DriveProgram.h"
#include "ErrorCodes.h"
#include "ErrorState.h"
#include "EventManager.h"
#include "GotoDriveStates.h"
#include "HelperStates.h"
#include "MainProcess.h"
#include "WelcomeMenuStates.h"

#include "Drivers/Display.h"
#include "Drivers/Keypad.h"





namespace
{



    //********************************************************************


    class GetGotoCoordinateState : public EnterIntMenuState
    {
    public:

        GetGotoCoordinateState( PGM_P title, GotoDriveAxis a, GotoDriveMode m, int initial );

        virtual State* onSelection( int value );

    private:

        GotoDriveAxis    mAxis;
        GotoDriveMode    mMode;
    };




    //********************************************************************


    class ReadyToGotoState : public YesOrNoState
    {
    public:
        ReadyToGotoState( PGM_P title, GotoDriveMode m, int axis1, int axis2 );

        virtual void onEntry();

        virtual State* onYes();
        virtual State* onNo();

    private:

        GotoDriveMode   mMode;
        int             mAxis1;
        int             mAxis2;
    };




    //********************************************************************


    class GetNumberRangeMenuState : public MenuState
    {
    public:

        GetNumberRangeMenuState( PGM_P title, GotoDriveAxis a, GotoDriveMode m );

    private:

        GotoDriveAxis    mAxis;
        GotoDriveMode    mMode;
    };











    //********************************************************************


    //                                                 1234567890123456
    const PROGMEM char sGetNumberRangeMenuTitleX[]  = "X Range (cm)?";
    const PROGMEM char sGetNumberRangeMenuTitleY[]  = "Y Range (cm)?";
    const PROGMEM char sGetNumberRangeMenuTitleN[]  = "N Range (cm)?";
    const PROGMEM char sGetNumberRangeMenuTitleE[]  = "E Range (cm)?";



    //                                            1234567890123456
    const PROGMEM char sGotoDriveMenuTitle[]   = "Set a Goto Tgt";
    const PROGMEM char sGotoDriveMenuItem00[]  = "Exit...";
    const PROGMEM char sGotoDriveMenuItem01[]  = "Relative Goto...";
    const PROGMEM char sGotoDriveMenuItem02[]  = "N & E Goto...";

    const PROGMEM char sGotoDriveMenuStart[]   = "Goto starts in:";


    const PROGMEM MenuList sGotoDriveMenu[] =
    {
        { sGotoDriveMenuItem01,  1 },
        { sGotoDriveMenuItem02,  2 },

        { sGotoDriveMenuItem00,  0 }
    };


    State* getGotoProgMenuState( uint8_t menuId, int8_t /* not used */ )
    {
        switch ( menuId )
        {
            case 0:
                return new WelcomeState;

            case 1:
                return new GetNumberRangeMenuState( sGetNumberRangeMenuTitleX,kFirstAxis, kRelative );

            case 2:
                return new GetNumberRangeMenuState( sGetNumberRangeMenuTitleN, kFirstAxis, kAbsolute );

            default:
                return 0;
        }
    }
}



GotoDriveMenuState::GotoDriveMenuState() :
MenuState( sGotoDriveMenuTitle, sGotoDriveMenu, sizeof( sGotoDriveMenu ) / sizeof( MenuItem ), getGotoProgMenuState, 0 )
{
    // Nothing else to do
}


void GotoDriveMenuState::onEntry()
{
    // TODO Prep GotoDrive here...
    MenuState::onEntry();
}







//********************************************************************




namespace
{

    const PROGMEM char sReadyToGoLbl[] = "GTo";


    //                              1234567890123456
    const PROGMEM char sAxisX[]  = "Downrange dist?";
    const PROGMEM char sAxisY[]  = "Crossrange dist?";
    const PROGMEM char sAxisN[]  = "North dist?";
    const PROGMEM char sAxisE[]  = "East dist?";


    //                                                 1234567890123456
    const PROGMEM char sGetNumberRangeMenuItem00[]  = "Exit...";
    const PROGMEM char sGetNumberRangeMenuItem01[]  = "  0-100...";
    const PROGMEM char sGetNumberRangeMenuItem02[]  = "100-200...";
    const PROGMEM char sGetNumberRangeMenuItem03[]  = "200-300...";
    const PROGMEM char sGetNumberRangeMenuItem04[]  = "300-400...";
    const PROGMEM char sGetNumberRangeMenuItem05[]  = "400-500...";


    const PROGMEM MenuList sGetNumberRangeDriveMenu[] =
    {
        { sGetNumberRangeMenuItem01,  1 },
        { sGetNumberRangeMenuItem02,  2 },
        { sGetNumberRangeMenuItem03,  3 },
        { sGetNumberRangeMenuItem04,  4 },
        { sGetNumberRangeMenuItem05,  5 },

        { sGetNumberRangeMenuItem00,  0 }
    };


    State* getGetNumberRangeMenuState( uint8_t menuId, int8_t param )
    {
        PGM_P menuTitleOptions[] = { sAxisX, sAxisY, sAxisN, sAxisE };
        PGM_P menuTitle = menuTitleOptions[ param ];

        GotoDriveAxis axis = static_cast<GotoDriveAxis>( param % 2 );
        GotoDriveMode mode = static_cast<GotoDriveMode>( param / 2 );

        switch ( menuId )
        {
            case 0:
                return new WelcomeState;

            case 1:
                return new GetGotoCoordinateState( menuTitle, axis, mode, 50 );

            case 2:
                return new GetGotoCoordinateState( menuTitle, axis, mode, 150 );

            case 3:
                return new GetGotoCoordinateState( menuTitle, axis, mode, 250 );

            case 4:
                return new GetGotoCoordinateState( menuTitle, axis, mode, 350 );

            case 5:
                return new GetGotoCoordinateState( menuTitle, axis, mode, 450 );

            default:
                return 0;
        }
    }


    GetNumberRangeMenuState::GetNumberRangeMenuState( PGM_P title, GotoDriveAxis a, GotoDriveMode m ) :
    MenuState( title, sGetNumberRangeDriveMenu, sizeof( sGetNumberRangeDriveMenu ) / sizeof( MenuItem ), getGetNumberRangeMenuState, a + 2*m ),
    mAxis( a ),
    mMode( m )
    {
        // Nothing else to do
    }






    //********************************************************************


    //  0123456789012345
    // "GTo sXXXa,sXXXa?";

    ReadyToGotoState::ReadyToGotoState( PGM_P title, GotoDriveMode m, int axis1, int axis2 ) :
    YesOrNoState( title ),
    mMode( m ),
    mAxis1( axis1 ),
    mAxis2( axis2 )
    {
        // Nothing else to do
    }

    void ReadyToGotoState::onEntry()
    {
        YesOrNoState::onEntry();

        // Display first goto coord

        bool isNeg1 = ( mAxis1 < 0 );
        int axis1 = abs( mAxis1 );
        uint8_t pos1 = 7;
        if ( axis1 >= 100 )
        {
            pos1 = 5;
        }
        else if ( axis1 >= 10 )
        {
            pos1 = 6;
        }
        Display::setCursor( 0, pos1 );
        Display::print( axis1 );

        if ( isNeg1 )
        {
            Display::setCursor( 0, pos1 - 1 );
            Display::print( '-' );
        }

        Display::setCursor( 0, 8 );
        if ( mMode == kRelative )
        {
            Display::print( 'x' );
        }
        else
        {
            Display::print( 'N' );
        }

        // Display second goto coord

        bool isNeg2 = ( mAxis2 < 0 );
        int axis2 = abs( mAxis2 );
        uint8_t pos2 = 13;
        if ( axis2 >= 100 )
        {
            pos2 = 11;
        }
        else if ( axis1 >= 10 )
        {
            pos2 = 12;
        }
        Display::setCursor( 0, pos2 );
        Display::print( axis2 );

        if ( isNeg2 )
        {
            Display::setCursor( 0, pos2 - 1 );
            Display::print( '-' );
        }

        Display::setCursor( 0, 14 );
        if ( mMode == kRelative )
        {
            Display::print( 'y' );
        }
        else
        {
            Display::print( 'E' );
        }

        // Done with coords

        Display::setCursor( 0, 15 );
        Display::print( '?' );
    }


    State* ReadyToGotoState::onYes()
    {
        // Add a little pause before program runs...
        Display::displayTopRowP16( sGotoDriveMenuStart );
        Display::clearBottomRow();
        for ( int8_t n = 5; n >= 0; --n )
        {
            Display::setCursor( 1, 0 );
            Display::print( n );
            CarrtCallback::yieldMilliseconds( 500 );
        }

        return new InitiateGotoDriveState( mMode, mAxis1, mAxis2 );
    }


    State* ReadyToGotoState::onNo()
    {
        return new GotoDriveMenuState;
    }




    //********************************************************************


    // Stash the 1st axis values here (no need to store 2nd axis
    int sAxis1Value;


    GetGotoCoordinateState::GetGotoCoordinateState( PGM_P title, GotoDriveAxis a, GotoDriveMode m, int initial ) :
    EnterIntMenuState( title, 20, 500, 10, initial ),
    mAxis( a ),
    mMode( m )
    {
        // Nothing else to do
    }


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    State* GetGotoCoordinateState::onSelection( int value )
    {
        PGM_P nextAxisTitle = mMode ? sGetNumberRangeMenuTitleE : sGetNumberRangeMenuTitleY;

        switch ( mAxis )
        {
            case kFirstAxis:
                sAxis1Value = value;
                return new GetNumberRangeMenuState( nextAxisTitle, kSecondAxis, mMode );
                break;

            case kSecondAxis:
                return new ReadyToGotoState( sReadyToGoLbl, mMode, sAxis1Value, value );
                break;
        }

        return 0;
    }

}





#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD
