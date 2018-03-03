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



#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD



#ifndef GotoDriveStates_h
#define GotoDriveStates_h



#include "State.h"

#include "PathSearch/Path.h"




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




class InitiateGotoDriveState : public State
{
public:

    InitiateGotoDriveState( GotoDriveMode mode, int goalAxis1, int goalAxis2 );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void convertInputsToAbsoluteCoords( int goalAxis1, int goalAxis2 );

    GotoDriveMode   mMode;
    int8_t          mCount;
};




class GotoDriveErrorState : public State
{
public:

    GotoDriveErrorState();

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

};




class DetermineNextWaypointState : public State
{
public:

    DetermineNextWaypointState();

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void doGlobalPathStage();
    void doGetBestGlobalWayPointStage();
    void doGetLocalPathStage();
    void doGetLongestDriveStage();
    void doUpdateDisplay();

    enum
    {
        kGetGlobalPathStage,
        kGetBestGlobalWayPointStage,
        kGetLocalPathStage,
        kGetLongestDriveStage,
        kDoneStage
    };

    PathFinder::Path*   mPath;
    int                 mOrigX;
    int                 mOrigY;
    int                 mTransferX;
    int                 mTransferY;
    uint8_t             mProgressStage;
};




class RotateTowardWaypointState : public State
{
public:

    RotateTowardWaypointState( int wayPointX, int wayPointY );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    int     mWayPointX;
    int     mWayPointY;


};












#endif // GotoDriveStates_h


#endif // CARRT_INCLUDE_GOTODRIVE_IN_BUILD




