/*
    WelcomeMenuStates.cpp - Welcome Menu States for CARRT

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





#include "WelcomeMenuStates.h"

#include <avr/pgmspace.h>

#include "EventManager.h"
#include "MainProcess.h"
#include "Menu.h"
#include "TestStates.h"

#if CARRT_INCLUDE_TESTS_IN_BUILD
#include "TestMenuStates.h"
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
#include "ProgDriveMenuStates.h"
#endif

#include "GotoDriveMenuStates.h"










namespace
{

    //                                        1234567890123456
    const PROGMEM char sWelcomeMenuTitle[] = "Welcome to CARRT";

#if CARRT_INCLUDE_TESTS_IN_BUILD
    const PROGMEM char sWelcomeMenuItem1[] = "Run Tests...";
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
    const PROGMEM char sWelcomeMenuItem2[] = "Prgm Drive...";
#endif

    const PROGMEM char sWelcomeMenuItem3[] = "GoTo Drive...";





    const PROGMEM MenuList sWelcomeMenu[] =
    {

#if CARRT_INCLUDE_TESTS_IN_BUILD
        { sWelcomeMenuItem1,    1 },
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
        { sWelcomeMenuItem2,    2 },
#endif

        { sWelcomeMenuItem3,    3 }

    };





    State* getWelcomeState( uint8_t menuId )
    {
        switch ( menuId )
        {

#if CARRT_INCLUDE_TESTS_IN_BUILD
            case 1:
                return new TestMenuState;
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
            case 2:
                return new ProgDriveState;
#endif

            case 3:
                return new GotoDriveState;

            default:
                return 0;
        }
    }

}





WelcomeState::WelcomeState() :
MenuState( sWelcomeMenuTitle, sWelcomeMenu, sizeof( sWelcomeMenu ) / sizeof( MenuItem ), getWelcomeState )
{
    // Nothing else to do
}






