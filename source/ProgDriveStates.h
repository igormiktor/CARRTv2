/*
    ProgDriveStates.h - Programmed Drive States for CARRT

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




#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD


#ifndef ProgDriveStates_h
#define ProgDriveStates_h



#include <stdint.h>

#include "State.h"



class BaseProgDriveState : public State
{
public:

    BaseProgDriveState();

    // onExit() - tear-down the state, but in these classes NEVER delete the case on Exit
    virtual void onExit();

    // Set the next State in the program
    BaseProgDriveState* setNextActionInProgram( BaseProgDriveState* next );

    // Get the next State in the program
    BaseProgDriveState* getNextActionInProgram()
    { return mNextStateInProgram; }

    // Change to the next state in program
    void gotoNextActionInProgram();

private:

    BaseProgDriveState*     mNextStateInProgram;
};









class PgmDrvDriveTimeState : public BaseProgDriveState
{
public:

    enum Direction
    {
        kForward,
        kReverse
    };

    PgmDrvDriveTimeState( Direction dir, uint8_t howManySecondsToDrive );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    Direction   mDirection;
    uint8_t     mSecondsToDrive;
    uint8_t     mElapsedSeconds;
    bool        mDriving;
};





#endif


#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD
