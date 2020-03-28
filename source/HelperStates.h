/*
    HelperStates.cpp - Helper states for CARRT states that need inputs

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




#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD || CARRT_INCLUDE_GOTODRIVE_IN_BUILD


#ifndef HelperStates_h
#define HelperStates_h

#include <stdint.h>

#include <avr/pgmspace.h>

#include "State.h"




class EnterIntMenuState : public State
{
public:

    EnterIntMenuState( PGM_P title, int min, int max, int inc, int initial );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

    virtual State* onSelection( int value ) = 0;

protected:

    void displayValue();

    PGM_P   mTitle;
    int     mMin;
    int     mMax;
    int     mInc;
    int     mInitial;
    int     mValue;
};






class YesOrNoState : public State
{
public:

    explicit YesOrNoState( PGM_P title );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

    virtual State* onYes() = 0;
    virtual State* onNo() = 0;

private:

    void displayYesNo();

    PGM_P   mTitle;
    bool    mYes;
};


#endif

#endif // CARRT_INCLUDE_PROGDRIVE_IN_BUILD || CARRT_INCLUDE_GOTODRIVE_IN_BUILD





