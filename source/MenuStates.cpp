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

#include "Drivers/Display.h"





void WelcomeState::onEntry()
{
    Display::clear();
    Display::displayTopRow( "Welcome to CARRT" );
    Display::displayBottomRow( "Hit any key..." );
}




void WelcomeState::onExit()
{
    Display::clear();
    delete this;
}




bool WelcomeState::onEvent( uint8_t event, int16_t param )
{
    return true;
}
