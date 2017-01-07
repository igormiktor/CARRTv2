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




#if CARRT_INCLUDE_TESTS_IN_BUILD


#ifndef TestStates_h
#define TestStates_h



#include <stdint.h>

#include "State.h"

#include "Utils/VectorUtils.h"




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
};



class CpuBatteryVoltageTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayVoltage();
};



class AvailableMemoryTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayMemory();

    bool mDisplayFreeMemory;
    bool mDisplaySRAM;
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

    int         mCurrentSlewAngle;
    int         mIncrement;
    uint8_t     mElapsedSeconds;
};




class CompassTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayCompassHeading();
};




class AccelerometerTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayAcceleration();

    bool    mNulled;
    uint8_t mAxis;
    char    mLabelX[14];
    char    mLabelY[14];
    char    mLabelZ[14];
    char    mLabelX0[17];
    char    mLabelY0[17];
    char    mLabelZ0[17];

    Vector3Float   mA0;
};




class GyroscopeTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void getAndDisplayAngularRates();

    bool    mNulled;
    uint8_t mAxis;
    char    mLabelX[15];
    char    mLabelY[15];
    char    mLabelZ[15];
    char    mLabelX0[16];
    char    mLabelY0[16];
    char    mLabelZ0[16];

    Vector3Float   mR0;
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
};






class ErrorTestState : public State
{
public:

    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    uint8_t mCount;
};





class NavigatorRotateTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displayNavInfo();
};





class NavigatorDriveTestState : public State
{
public:

    virtual void onEntry();
    virtual void onExit();
    virtual bool onEvent( uint8_t event, int16_t param );

private:

    void displayNavInfo();

    enum
    {
        kReadyToGo,
        kGoing,
        kDisplaying
    };

    uint8_t mStatus;

    enum
    {
        kFwd,
        kRev
    };

    uint8_t mNextDirection;

    int8_t mDriveTime;
};



class GreetTestState : public State
{
public:
    virtual void onEntry();
    virtual bool onEvent( uint8_t event, int16_t param );
};



#endif


#endif  // CARRT_INCLUDE_TESTS_IN_BUILD
