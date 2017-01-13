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
        kReverse,
        kRotateLeft,
        kRotateRight
    };

    PgmDrvDriveTimeState( Direction dir, uint8_t howManySecondsToDrive );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displaySeconds();

    Direction   mDirection;
    uint8_t     mQtrSecondsToDrive;
    uint8_t     mElapsedQtrSeconds;
    bool        mDriving;
};






//******************************************************************


class PgmDrvPauseState : public BaseProgDriveState
{
public:

    explicit PgmDrvPauseState( uint8_t howManySecondsToPause );

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displaySeconds();

    uint8_t     mQtrSecondsToPause;
    uint8_t     mElapsedQtrSeconds;
};






//******************************************************************


class PgmDrvBeepState : public BaseProgDriveState
{
public:

    explicit PgmDrvBeepState( uint8_t howManySecondsToBeep );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displaySeconds();

    uint8_t     mQtrSecondsToBeep;
    uint8_t     mElapsedQtrSeconds;
};






//******************************************************************


// cppcheck-suppress noConstructor
class PgmDrvScanState : public BaseProgDriveState
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displayAngleRange();
    bool updateSlewAngle();

    int mCurrentSlewAngle;
};






//******************************************************************


class PgmDrvRotAngleState : public BaseProgDriveState
{
public:

    explicit PgmDrvRotAngleState( int rotationAngle );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void reverseDirection();
    bool rotationDone( int currHeading );
    void displayProgress( int currHeading );

    int     mRotationAngle;
    int     mTargetHeading;
    int     mPriorLeftToGo;
    bool    mGoLeft;
};






//******************************************************************


class PgmDrvDriveDistanceState : public BaseProgDriveState
{
public:

    enum
    {
        kForward,
        kReverse
    };

    PgmDrvDriveDistanceState( uint8_t direction, uint8_t distInCm );

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displayDistance();

    uint8_t     mDistance;
    uint8_t     mQtrSecondsToDrive;
    uint8_t     mElapsedQtrSeconds;
    bool        mGoForward;
    bool        mDriving;
};





#endif


#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD
