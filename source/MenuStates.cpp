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
        { sWelcomeMenuItem1,   1 },
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




void TestMenuState::onEntry()
{
    Display::displayTopRow( "Test Menu" );
    Display::displayBottomRow( "Nothing yet..." );
}


bool TestMenuState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new WelcomeState );
    }
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




