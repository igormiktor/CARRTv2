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

    const PROGMEM char sWelcomeMenuTitle[] = "Welcome to CARRT";
    const PROGMEM char sWelcomeMenuItem0[] = "Run Tests";
    const PROGMEM char sWelcomeMenuItem1[] = "Enter GoTo";

    const PROGMEM MenuList sWelcomeMenu[] =
    {
        { sWelcomeMenuItem0,   0 },
        { sWelcomeMenuItem1,   1 }
    };

    State* getWelcomeState( uint8_t id )
    {
        switch ( id )
        {
            case 0:
                return new TestMenuState;

            case 1:
                return new RunMenuState;

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








namespace
{

    const PROGMEM char sTestMenuTitle[] = "Select Test";
    const PROGMEM char sTestMenuItem0[] = "1/4 Sec Events";
    const PROGMEM char sTestMenuItem1[] = "1 Sec Events";
    const PROGMEM char sTestMenuItem2[] = "8 Sec Events";
    const PROGMEM char sTestMenuItem3[] = "Beep";
    const PROGMEM char sTestMenuItem4[] = "Temp Sensor";
    const PROGMEM char sTestMenuItem5[] = "Batt LED";
    const PROGMEM char sTestMenuItem6[] = "Motor Batt";
    const PROGMEM char sTestMenuItem7[] = "CPU Batt";
    const PROGMEM char sTestMenuItem8[] = "Memory";
    const PROGMEM char sTestMenuItem9[] = "Back...";

    const PROGMEM MenuList sTestMenu[] =
    {
        { sTestMenuItem0,     0 },
        { sTestMenuItem1,     1 },
        { sTestMenuItem2,     2 },
        { sTestMenuItem3,     3 },
        { sTestMenuItem4,     4 },
        { sTestMenuItem5,     5 },
        { sTestMenuItem6,     6 },
        { sTestMenuItem7,     7 },
        { sTestMenuItem8,     8 },
        { sTestMenuItem9,     9 },
    };

    State* getTestState( uint8_t id )
    {
        switch ( id )
        {
            case 0:
                return new Event1_4TestState;

            case 1:
                return new Event1TestState;

            case 2:
                return new Event8TestState;

            case 3:
                return new BeepTestState;

            case 4:
                return new TempSensorTestState;

            case 5:
                return new BatteryLedTestState;

            case 6:
                return new MotorBatteryVoltageTestState;

            case 7:
                return new CpuBatteryVoltageTestState;

            case 8:
                return new AvailableMemoryTestState;

            case 9:
                return new WelcomeState;

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




















void RunMenuState::onEntry()
{
    Display::displayTopRow( "Run Menu" );
    Display::displayBottomRow( "Forthcoming..." );
}


bool RunMenuState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new WelcomeState );
    }
}




