/*
    MenuStates.cpp - Menu States for CARRT

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





#include "MenuStates.h"

#include <avr/pgmspace.h>

#include "EventManager.h"
#include "MainProcess.h"
#include "Menu.h"
#include "TestStates.h"


#include "Drivers/Display.h"
#include "Drivers/Keypad.h"





MenuState::MenuState( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f ) :
mMenu( menuName, menuList, nbrItems, f )
{
    // Nothing else to do
}



void MenuState::onEntry()
{
    mMenu.init();
}


bool MenuState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Up || button & Keypad::kButton_Left )
        {
            mMenu.previous();
        }
        if ( button & Keypad::kButton_Down || button & Keypad::kButton_Right )
        {
            mMenu.next();
        }
        if ( button & Keypad::kButton_Select )
        {
            State* newState = mMenu.selected();
            if ( newState )
            {
                MainProcess::changeState( newState );
            }
        }
    }

    return true;
}









namespace
{

    //                                        1234567890123456
    const PROGMEM char sWelcomeMenuTitle[] = "Welcome to CARRT";
#if CARRT_INCLUDE_TESTS_IN_BUILD
    const PROGMEM char sWelcomeMenuItem1[] = "Run Tests...";
#endif
#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
    const PROGMEM char sWelcomeMenuItem2[] = "Prog a Drive...";
#endif
    const PROGMEM char sWelcomeMenuItem3[] = "Enter a GoTo...";



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







#if CARRT_INCLUDE_TESTS_IN_BUILD

namespace
{
    //                                      1234567890123456
    const PROGMEM char sTestMenuTitle[]  = "Select Test";
    const PROGMEM char sTestMenuItem00[] = "Back...";
    const PROGMEM char sTestMenuItem01[] = "Memory";
    const PROGMEM char sTestMenuItem02[] = "1/4 Sec Events";
    const PROGMEM char sTestMenuItem03[] = "1 Sec Events";
    const PROGMEM char sTestMenuItem04[] = "8 Sec Events";
    const PROGMEM char sTestMenuItem05[] = "Beep";
    const PROGMEM char sTestMenuItem06[] = "Temp Sensor";
    const PROGMEM char sTestMenuItem07[] = "Batt LEDs";
    const PROGMEM char sTestMenuItem08[] = "Motor Batt";
    const PROGMEM char sTestMenuItem09[] = "CPU Batt";
    const PROGMEM char sTestMenuItem10[] = "Range Scan";
    const PROGMEM char sTestMenuItem11[] = "Compass";
    const PROGMEM char sTestMenuItem12[] = "Accelerometer";
    const PROGMEM char sTestMenuItem13[] = "Gyroscope";
    const PROGMEM char sTestMenuItem14[] = "Drive Fwd/Rev";
    const PROGMEM char sTestMenuItem15[] = "Drive Left/Right";
    const PROGMEM char sTestMenuItem16[] = "Error Handling";

    const PROGMEM MenuList sTestMenu[] =
    {
        { sTestMenuItem01,  1 },
        { sTestMenuItem02,  2 },
        { sTestMenuItem03,  3 },
        { sTestMenuItem04,  4 },
        { sTestMenuItem05,  5 },
        { sTestMenuItem06,  6 },
        { sTestMenuItem07,  7 },
        { sTestMenuItem08,  8 },
        { sTestMenuItem09,  9 },
        { sTestMenuItem10,  10 },
        { sTestMenuItem11,  11 },
        { sTestMenuItem12,  12 },
        { sTestMenuItem13,  13 },
        { sTestMenuItem14,  14 },
        { sTestMenuItem15,  15 },
        { sTestMenuItem16,  16 },

        { sTestMenuItem00,  0 }
    };

    State* getTestState( uint8_t menuId )
    {
        switch ( menuId )
        {
            case 0:
                return new WelcomeState;

            case 1:
                return new AvailableMemoryTestState;

            case 2:
                return new Event1_4TestState;

            case 3:
                return new Event1TestState;

            case 4:
                return new Event8TestState;

            case 5:
                return new BeepTestState;

            case 6:
                return new TempSensorTestState;

            case 7:
                return new BatteryLedTestState;

            case 8:
                return new MotorBatteryVoltageTestState;

            case 9:
                return new CpuBatteryVoltageTestState;

            case 10:
                return new RangeScanTestState;

            case 11:
                return new CompassTestState;

            case 12:
                return new AccelerometerTestState;

            case 13:
                return new GyroscopeTestState;

            case 14:
                return new MotorFwdRevTestState;

            case 15:
                return new MotorLeftRightTestState;

            case 16:
                return new ErrorTestState;

            default:
                return 0;
        }
    }
}





TestMenuState::TestMenuState() :
MenuState( sTestMenuTitle, sTestMenu, sizeof( sTestMenu ) / sizeof( MenuItem ), getTestState )
{
    // Nothing else to do
}


#endif  // CARRT_INCLUDE_TESTS_IN_BUILD






//******************************************************************



#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD

void ProgDriveState::onEntry()
{
    Display::displayTopRow( "Prog Drive Menu" );
    Display::displayBottomRow( "Forthcoming..." );
}


bool ProgDriveState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new WelcomeState );
    }
}


#endif  // CARRT_INCLUDE_TESTS_IN_BUILD








//******************************************************************




void GotoDriveState::onEntry()
{
    Display::displayTopRow( "GoTo Drive Menu" );
    Display::displayBottomRow( "Forthcoming..." );
}


bool GotoDriveState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new WelcomeState );
    }
}




