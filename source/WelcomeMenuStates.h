/*
    WelcomeMenuStates.h - Welcome Menu States for CARRT

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





#ifndef WelcomeMenuStates_h
#define WelcomeMenuStates_h


#include "MenuState.h"

#include "Menu.h"




class WelcomeState : public MenuState
{
public:

    WelcomeState();
};



class AboutState : public State
{
public:
    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t button );

private:
    void displayInfo();

    enum
    {
        kVersion,
        kBuild,
        kGitTag,
        kCredits,

#if CARRT_INCLUDE_SPECIAL_MSG
        kSpecial,
#endif

        kLast
    };

    uint8_t mDisplayMode;
};



#endif

