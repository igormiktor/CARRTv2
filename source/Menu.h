/*
    Menu.h - Menu mechanism for CARRT (with menus in program (flash) memory)

    Copyright (c) 2022 Igor Mikolic-Torreira.  All right reserved.

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





#ifndef Menu_h
#define Menu_h


#include <stdint.h>

#include <avr/pgmspace.h>


class State;


struct MenuItem
{
    PGM_P              mMenuLabel;
    uint8_t            mStateId;
};

typedef MenuItem   MenuList;

typedef State* (*StateSelector)( uint8_t, int8_t );



class Menu
{
public:

    Menu( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f, int8_t param  );

    void init();

    // Display the current item
    void displayItem();
    void hide();
    void show();

    void next();
    void previous();

    uint8_t selectedId();
    State* selectedState();

private:

    StateSelector       mGetStateFromId;
    PGM_P               mMenuName;
    const MenuList*     mMenuList;
    uint8_t             mCurrentItem;
    uint8_t             mNbrItems;
    int8_t              mParam;
};


#endif
