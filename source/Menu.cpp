/*
    Menu.cpp - Menu mechanism for CARRT (with menus in program (flash) memory)

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





#include "Menu.h"

#include <stdint.h>

#include <avr/pgmspace.h>

#include "Drivers/Display.h"

#include "Utils/DebuggingMacros.h"



Menu::Menu( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f  ) :
mGetState( f ),
mMenuName( menuName ),
mMenuList( menuList ),
mCurrentItem( 0 ),
mNbrItems( nbrItems )
{
    // Nothing else to do
}



void Menu::init()
{
    mCurrentItem = 0;
    Display::clear();
    Display::displayOn();
    char tmp[17];
    strcpy_P( tmp, mMenuName );
    Display::displayTopRow( tmp );
    displayItem();
}



void Menu::displayItem()
{
    char tmp[17];
    strcpy_P( tmp, reinterpret_cast<PGM_P>( pgm_read_word( &(mMenuList[mCurrentItem].mMenuLabel) ) ) );
    Display::displayBottomRow( tmp );
}


void Menu::hide()
{
    Display::displayOff();
}


void Menu::show()
{
    Display::displayOn();
}


void Menu::next()
{
    ++mCurrentItem;
    mCurrentItem %= mNbrItems;
    displayItem();
}


void Menu::previous()
{
    --mCurrentItem;
    mCurrentItem += mNbrItems;
    mCurrentItem %= mNbrItems;
    displayItem();
}



State* Menu::selected()
{
    uint8_t stateId = pgm_read_byte( &(mMenuList[mCurrentItem].mStateId) );
    return mGetState( stateId );
}

