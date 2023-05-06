/*
    State.cpp - Base class for CARRT's states

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





#include "State.h"




// onEntry() - Set up the state
void State::onEntry()
{
    // Default does nothing
}



// onExit() - Tear-down the state.  It should delete the state, if appropriate
void State::onExit()
{
    // Default deletes itself
    delete this;
}



// pause() - Pause the state. Preserve status and stop mechanical and other actions
void State::pause()
{
    // Default does nothing
}



// unpause() - Unpause (continue, re-animate) the state. Restore status and restart mechanical and other actions
void State::unpause()
{
    // Default does nothing
}



// onEvent() - Deal with an event; return true to give other handlers an chance to
// process the event; return false to allow no further event handling
bool State::onEvent( uint8_t event, int16_t param )
{
    return true;
}



// Virtual Destructor
State::~State()
{
    // Nothing to do; just need it to be virtual
}
