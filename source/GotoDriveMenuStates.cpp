/*
    GotoDriveMenuStates.cpp - Goto Drive Menu States for CARRT

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




#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD



#include "GotoDriveMenuStates.h"

#include <avr/pgmspace.h>

#include "CarrtCallback.h"
#include "DriveProgram.h"
#include "ErrorCodes.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "WelcomeMenuStates.h"

#include "Drivers/Display.h"
#include "Drivers/Keypad.h"







namespace
{
    //                                              1234567890123456
    const PROGMEM char sGotoDriveMenuTitle[]   = "Set a GoTo Tgt";
    const PROGMEM char sGotoDriveMenuItem00[]  = "Exit...";
    const PROGMEM char sGotoDriveMenuItem01[]  = "Relative GoTo...";
    const PROGMEM char sGotoDriveMenuItem02[]  = "N & E GoTo...";
    const PROGMEM char sGotoDriveMenuItem03[]  = "Go to GoTo...";
    const PROGMEM char sGotoDriveMenuItem04[]  = "Clear...";

    const PROGMEM char sGotoDriveMenuStart[]   = "Starting in...";


    const PROGMEM MenuList sGotoDriveMenu[] =
    {
        { sGotoDriveMenuItem01,  1 },
        { sGotoDriveMenuItem02,  2 },
        { sGotoDriveMenuItem03,  3 },
        { sGotoDriveMenuItem04,  4 },

        { sGotoDriveMenuItem00,  0 }
    };



    State* prepFirstActionInProgram()
    {
#if 0
        // Add a little pause before program runs...
        Display::displayTopRowP16( sGotoDriveMenuStart );
        Display::clearBottomRow();
        for ( int8_t n = 3; n >= 0; --n )
        {
            Display::setCursor( 1, 0 );
            Display::print( n );
            CarrtCallback::yieldMilliseconds( 500 );
        }

        State* progStartState = DriveProgram::getProgramStart();
        if ( !progStartState )
        {
            MainProcess::postErrorEvent( kNullStateInProgram );

            // TODO Replace with the right starting state...
            progStartState = new WelcomeState;
        }
        return progStartState;
#endif
        return new WelcomeState;
    }



    State* getGotoProgMenuState( uint8_t menuId )
    {
        switch ( menuId )
        {
            case 0:
                return new WelcomeState;

            case 1:
                return new WelcomeState;

            case 2:
                return new WelcomeState;

            case 3:
                return new WelcomeState;

            case 4:
                return new WelcomeState;

            default:
                return 0;
        }
    }
}



GotoDriveMenuState::GotoDriveMenuState() :
MenuState( sGotoDriveMenuTitle, sGotoDriveMenu, sizeof( sGotoDriveMenu ) / sizeof( MenuItem ), getGotoProgMenuState )
{
    // Nothing else to do
}


void GotoDriveMenuState::onEntry()
{
    // Prep GotoDrive here...
    MenuState::onEntry();
}





#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD
