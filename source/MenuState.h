/*
    MenuState.h - Base Class for Menu States for CARRT

    Copyright (c) 2020 Igor Mikolic-Torreira.  All right reserved.

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





#ifndef MenuState_h
#define MenuState_h


#include <avr/pgmspace.h>

#include "State.h"

#include "Menu.h"



class MenuState : public State
{
public:

    MenuState( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f, int8_t param );

    virtual void onEntry();

    virtual bool onEvent( uint8_t event, int16_t param );

    void goMenuPrevious()       { mMenu.previous(); }
    void goMenuNext()           { mMenu.next(); }

    uint8_t getMenuSelectedId()         { return mMenu.selectedId(); }
    State* getMenuSelectedState()       { return mMenu.selectedState(); }


private:

    int8_t  mParam;
    Menu    mMenu;
};



#endif

