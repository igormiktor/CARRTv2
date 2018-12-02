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

#include "TestMenuStates.h"
#include "ProgDriveMenuStates.h"
#include "GotoDriveMenuStates.h"

#include "Drivers/Display.h"
#include "Drivers/Keypad.h"









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

#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD
    const PROGMEM char sWelcomeMenuItem3[] = "Goto Drive...";
#endif

    const PROGMEM char sWelcomeMenuItem4[] = "About...";



    const PROGMEM MenuList sWelcomeMenu[] =
    {

#if CARRT_INCLUDE_TESTS_IN_BUILD
        { sWelcomeMenuItem1,    1 },
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
        { sWelcomeMenuItem2,    2 },
#endif

#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD
        { sWelcomeMenuItem3,    3 },
#endif

        { sWelcomeMenuItem4,    4 }

    };





    State* getWelcomeState( uint8_t menuId, int8_t /* not used */ )
    {
        switch ( menuId )
        {

#if CARRT_INCLUDE_TESTS_IN_BUILD
            case 1:
                return new TestMenuState;
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
            case 2:
                return new ProgDriveProgramMenuState;
#endif

#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD
            case 3:
                return new GotoDriveMenuState;
#endif

            case 4:
                return new AboutState;

            default:
                return 0;
        }
    }
}








WelcomeState::WelcomeState() :
MenuState( sWelcomeMenuTitle, sWelcomeMenu, sizeof( sWelcomeMenu ) / sizeof( MenuItem ), getWelcomeState, 0 )
{
    // Nothing else to do
}









//***********************************************************************


#if CARRT_INCLUDE_TESTS_IN_BUILD
#define CARRT_TEST_STR  "T"
#else
#define CARRT_TEST_STR  ""
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
#define CARRT_PROGDRV_STR  "P"
#else
#define CARRT_PROGDRV_STR  ""
#endif

#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD
#define CARRT_GOTODRV_STR  "G"
#else
#define CARRT_GOTODRV_STR  ""
#endif

#if CARRT_NAVIGATE_USING_INERTIAL
#define CARRT_NAV_STR  "IMU"
#elif CARRT_NAVIGATE_USING_DEADRECKONING
#define CARRT_NAV_STR  "DR"
#else
#error "One of CARRT_NAVIGATE_USING_INERTIAL or CARRT_NAVIGATE_USING_DEADRECKONING must be defined."
#endif

#if CARRT_ENABLE_DEBUG_SERIAL
#define CARRT_DEBUG_STR  "D"
#else
#define CARRT_DEBUG_STR  ""
#endif


#define CARRT_FEATURES   CARRT_GOTODRV_STR " " CARRT_PROGDRV_STR " " CARRT_TEST_STR " " CARRT_NAV_STR " " CARRT_DEBUG_STR



void AboutState::onEntry()
{
    mDisplayMode = kVersion;
    displayInfo();

}


bool AboutState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new WelcomeState );
        }
        else if ( button & Keypad::kButton_Right || button & Keypad::kButton_Up )
        {
            // Toggle display forward
            ++mDisplayMode;
            mDisplayMode %= kLast;
            displayInfo();
        }
        else if ( button & Keypad::kButton_Left || button & Keypad::kButton_Down )
        {
            // Toggle display backward
            --mDisplayMode;
            mDisplayMode += kLast;
            mDisplayMode %= kLast;
            displayInfo();
       }
    }

    return true;
}


void AboutState::displayInfo()
{
    Display::clear();
    switch ( mDisplayMode )
    {
        default:
        case kVersion:
            //                                   1234567890123456
            Display::displayTopRowP16(    PSTR( "v" CARRT_VERSION ) );
            Display::displayBottomRowP16( PSTR( " " CARRT_FEATURES ) );
            break;

        case kBuild:
            //                                   1234567890123456
            //                                   mmm dd yyyy
            //
            Display::displayTopRowP16(    PSTR( __DATE__ ) );
            Display::displayBottomRowP16( PSTR( __TIME__ ) );
            break;

        case kCredits:
            //                                   1234567890123456
            Display::displayTopRowP16(    PSTR( "HW & SW by" ) );
            Display::displayBottomRowP16( PSTR( "Igor" ) );
            break;

#if CARRT_INCLUDE_SPECIAL_MSG
        case kSpecial:
            //                                   1234567890123456
            Display::displayTopRowP16(    PSTR( "Hi to..." ) );
            Display::displayBottomRowP16( PSTR( CARRT_SPECIAL_MSG_TEXT ) );
            break;
#endif
    }

}

