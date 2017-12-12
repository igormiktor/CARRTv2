/*
    GotoDriveStates.h - Goto Drive States for CARRT

    Copyright (c) 2017 Igor Mikolic-Torreira.  All right reserved.

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



#ifndef GotoDriveStates_h
#define GotoDriveStates_h



#include "State.h"




enum GotoDriveAxis
{
    kFirstAxis,
    kSecondAxis
};


enum GotoDriveMode
{
    kRelative,
    kAbsolute
};




class DeterminePathToGoalState : public State
{
public:

    DeterminePathToGoalState();
    DeterminePathToGoalState( GotoDriveMode mode, int goalAxis1, int goalAxis2 );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    int             mGoalX;
    int             mGoalY;
    GotoDriveMode   mMode;
};









#endif // GotoDriveStates_h
