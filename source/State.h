/*
    State.h - Base class for CARRT's states

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





#ifndef State_h
#define State_h

#include <inttypes.h>


class State
{
public:

    // onEntry() - Set up the state
    virtual void onEntry();

    // onExit() - Tear-down the state.  It should delete the state, if appropriate
    virtual void onExit();

    // onPause() - Pause the state. Preserve status and stop mechanical and other actions
    virtual void pause();

    // onContinue() - Continue (re-animate) the state. Restore status and restart mechanical and other actions
    virtual void unpause();

    // onEvent() - Deal with an event; return true to give other handlers an chance to
    // process the event; return false to allow no further event handling
    virtual bool onEvent( uint8_t event, int16_t param );

    // Virtual destructor
    virtual ~State();

};


#endif
