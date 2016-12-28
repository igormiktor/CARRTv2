/*
    ProgDriveMenuStates.h - Programmed Drive Menu States for CARRT

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

#ifndef ProgDriveMenuStates_h
#define ProgDriveMenuStates_h




#include "MenuState.h"

#include "DriveProgram.h"
#include "ProgDriveStates.h"











class ProgDriveProgramMenuState : public MenuState
{
public:

    ProgDriveProgramMenuState();

};







//********************************************************************


class ProgDriveAnyTimeMenuState : public State
{
public:

    enum Action
    {
        kForward,
        kReverse,
        kRotateLeft,
        kRotateRight,
        kPause,
        kBeep
    };

    ProgDriveAnyTimeMenuState( Action action );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displaySecondsSetting();

    const Action    mAction;
    uint8_t         mSeconds;
};








//********************************************************************


class ProgDriveFwdTimeMenuState : public ProgDriveAnyTimeMenuState
{
public:

    ProgDriveFwdTimeMenuState();
};



class ProgDriveRevTimeMenuState : public ProgDriveAnyTimeMenuState
{
public:

    ProgDriveRevTimeMenuState();
};



class ProgDriveRotLTimeMenuState : public ProgDriveAnyTimeMenuState
{
public:

    ProgDriveRotLTimeMenuState();
};



class ProgDriveRotRTimeMenuState : public ProgDriveAnyTimeMenuState
{
public:

    ProgDriveRotRTimeMenuState();
};







//********************************************************************


class ProgDriveAddRotateMenuState;
class ProgDriveAddPauseMenuState;
class ProgDriveAddBeepMenuState;
class ProgDriveAddScanMenuState;


#if 0
class ProgDriveRunMenuState : public ProgDriveMenuBaseState
{
public:

    explicit ProgDriveRunMenuState( BaseProgDriveState* program );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );
};





class ProgDriveInterruptMenuState : public ProgDriveMenuBaseState
{
public:

    explicit ProgDriveInterruptMenuState( BaseProgDriveState* program );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );
};





class ProgDriveTearDownMenuState : public ProgDriveMenuBaseState
{
public:

    explicit ProgDriveTearDownMenuState( BaseProgDriveState* program );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );
};

#endif



#endif

#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD

