/*
    MenuStates.h - Menu States for CARRT

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





#ifndef MenuStates_h
#define MenuStates_h


#include "State.h"

#include "Menu.h"



class MenuState : public State
{
public:

    MenuState( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f );

    virtual void onEntry();

    virtual bool onEvent( uint8_t event, int16_t param );


private:

    Menu    mMenu;
};





class WelcomeState : public MenuState
{
public:

    WelcomeState();
};




class TestMenuState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

};



class RunMenuState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );
};


#endif

