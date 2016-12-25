/*
    TestStates.h - Test States for CARRT

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




#ifndef TestStates_h
#define TestStates_h


#include "State.h"

#include <stdint.h>




class Event1_4TestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    int mCount;
};



class Event1TestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    int mCount;
};



class Event8TestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    int mCount;
};



class BeepTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    bool mBeepOn;
};



class TempSensorTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayTemp();
    bool mTempF;
    char mTempLabel[8];

};



class BatteryLedTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void setBatteryLed( int nbr, bool state );

    int8_t      mCurrentLed;
};



class MotorBatteryVoltageTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayVoltage();

    char mLabel[5];
};



class CpuBatteryVoltageTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayVoltage();

    char mLabel[5];
};



class AvailableMemoryTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayMemory();

    char mLabelUsed[12];
    char mLabelFree[12];
    bool mDisplayFreeMemory;
};





class RangeScanTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayRange();
    void updateSlewAngle();

    int mCurrentSlewAngle;
    int mIncrement;
    char mLabelRng[7];
};




class CompassTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayCompassHeading();

    char mLabelHdg[7];
};




class AccelerometerTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayAcceleration();

    uint8_t mAxis;
    char    mLabelX[10];
    char    mLabelY[10];
    char    mLabelZ[10];
};




class GyroscopeTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayAngularRates();

    uint8_t mAxis;
    char    mLabelX[13];
    char    mLabelY[13];
    char    mLabelZ[13];
};





class MotorFwdRevTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void updateDriveStatus();

    enum
    {
        kStopped,
        kFwd,
        kPause,
        kRev
    };

    uint8_t mDriveStatus;
    uint8_t mElapsedSeconds;
    char    mLabelFwd[8];
    char    mLabelRev[8];
    char    mLabelStop[8];
};





class MotorLeftRightTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void updateDriveStatus();

    enum
    {
        kStopped,
        kLeft,
        kPause,
        kRight
    };

    uint8_t mDriveStatus;
    uint8_t mElapsedSeconds;
    char    mLabelLeft[5];
    char    mLabelRight[6];
    char    mLabelStop[8];
};









#endif

