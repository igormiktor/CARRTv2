/*
    MenuState.cpp - Base Class for Menu States for CARRT

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





#include "MenuState.h"

#include <avr/pgmspace.h>

#include "EventManager.h"
#include "MainProcess.h"
#include "Menu.h"

#include "Drivers/Keypad.h"




MenuState::MenuState( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f, int8_t param ) :
mMenu( menuName, menuList, nbrItems, f, param )
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
            goMenuPrevious();
        }
        if ( button & Keypad::kButton_Down || button & Keypad::kButton_Right )
        {
            goMenuNext();
        }
        if ( button & Keypad::kButton_Select )
        {
            // changeState() protects us against null states
            MainProcess::changeState( mMenu.selectedState() );
        }
    }

    return true;
}






