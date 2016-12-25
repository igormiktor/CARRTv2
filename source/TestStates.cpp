/*
    TestStates.cpp - Test States for CARRT

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



#include "TestStates.h"

#include <avr/pgmspace.h>

#include "AVRTools/SystemClock.h"
#include "AVRTools/MemUtils.h"

#include "CarrtCallback.h"
#include "CarrtPins.h"
#include "ErrorCodes.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "MenuStates.h"

#include "Drivers/Battery.h"
#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/Keypad.h"
#include "Drivers/L3GD20.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/Motors.h"
#include "Drivers/Radar.h"
#include "Drivers/TempSensor.h"






/******************************************/


void Event1_4TestState::onEntry()
{
    mCount = 0;

    char tmp[17];
    strcpy_P( tmp, PSTR( "1/4s Event Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );
    Display::setCursor( 1, 0 );
    Display::print( 0 );
}


bool Event1_4TestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent )
    {
        Display::clearBottomRow();
        Display::setCursor( 1, 0 );
        Display::print( ++mCount );
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}





/******************************************/


void Event1TestState::onEntry()
{
    mCount = 0;

    char tmp[17];
    strcpy_P( tmp, PSTR( "1s Event Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );
    Display::setCursor( 1, 0 );
    Display::print( 0 );
}


bool Event1TestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        Display::clearBottomRow();
        Display::setCursor( 1, 0 );
        Display::print( ++mCount );
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}






/******************************************/


void Event8TestState::onEntry()
{
    mCount = 0;

    char tmp[17];
    strcpy_P( tmp, PSTR( "8s Event Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );
    Display::setCursor( 1, 0 );
    Display::print( 0 );
}


bool Event8TestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kEightSecondTimerEvent )
    {
        Display::clearBottomRow();
        Display::setCursor( 1, 0 );
        Display::print( ++mCount );
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}





/******************************************/


void BeepTestState::onEntry()
{
    Beep::beep();

    char tmp[17];
    strcpy_P( tmp, PSTR( "Beep Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    mBeepOn = false;
}


void BeepTestState::onExit()
{
    Beep::beepOff();

    delete this;
}


bool BeepTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kQuarterSecondTimerEvent && param % 2 )
    {
        // toggle beep every half second
        if ( mBeepOn )
        {
            Beep::beepOff();
            mBeepOn = false;
        }
        else
        {
            Beep::beepOn();
            mBeepOn = true;
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Beep::beepOff();
        mBeepOn = false;
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}








/******************************************/


void TempSensorTestState::onEntry()
{
    mTempF = false;

    char tmp[17];
    strcpy_P( tmp, PSTR( "Temp Sensor Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    strcpy_P( mTempLabel, PSTR( "Temp = " ) );

    getAndDisplayTemp();
}


bool TempSensorTestState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        getAndDisplayTemp();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new TestMenuState );
        }
        else
        {
            // Any other key, toggle units
            mTempF = !mTempF;
            getAndDisplayTemp();
        }
    }

    return true;
}


void TempSensorTestState::getAndDisplayTemp()
{
    float temp = mTempF ? TempSensor::getTempF() : TempSensor::getTempC();
    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::print( mTempLabel );
    Display::setCursor( 1, 7 );
    Display::print( temp );
    Display::setCursor( 1, 13 );
    Display::print( mTempF ? 'F' : 'C' );
}








/******************************************/


void BatteryLedTestState::onEntry()
{
    setGpioPinHigh( pBatteryMotorLedGreenPin );
    setGpioPinHigh( pBatteryMotorLedYellowPin );
    setGpioPinHigh( pBatteryMotorLedRedPin );
    setGpioPinHigh( pBatteryElectronicsLedGreenPin );
    setGpioPinHigh( pBatteryElectronicsLedYellowPin );
    setGpioPinHigh( pBatteryElectronicsLedRedPin );

    mCurrentLed = 0;

    char tmp[17];
    strcpy_P( tmp, PSTR( "Batt LED Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    delayMilliseconds( 500 );

    setGpioPinLow( pBatteryMotorLedGreenPin );
    setGpioPinLow( pBatteryMotorLedYellowPin );
    setGpioPinLow( pBatteryMotorLedRedPin );
    setGpioPinLow( pBatteryElectronicsLedGreenPin );
    setGpioPinLow( pBatteryElectronicsLedYellowPin );
    setGpioPinLow( pBatteryElectronicsLedRedPin );
}


void BatteryLedTestState::onExit()
{
    setBatteryLed( mCurrentLed, kDigitalLow );

    delete this;
}


bool BatteryLedTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        setBatteryLed( mCurrentLed, kDigitalLow );
        ++mCurrentLed;
        mCurrentLed %= 6;
        setBatteryLed( mCurrentLed, kDigitalHigh );
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    // Returning false short-circuits further event handling
    // which prevents the default handlers from adjusting LEDs
    return false;
}


void BatteryLedTestState::setBatteryLed( int nbr, bool state )
{
    switch ( nbr )
    {
        case 0:
            writeGpioPinDigital( pBatteryMotorLedGreenPin, state );
            break;

        case 1:
            writeGpioPinDigital( pBatteryMotorLedYellowPin, state );
            break;

        case 2:
            writeGpioPinDigital( pBatteryMotorLedRedPin, state );
            break;

        case 3:
            writeGpioPinDigital( pBatteryElectronicsLedGreenPin, state );
            break;

        case 4:
            writeGpioPinDigital( pBatteryElectronicsLedYellowPin, state );
            break;

        case 5:
            writeGpioPinDigital( pBatteryElectronicsLedRedPin, state );
            break;
    }
}





/******************************************/


void MotorBatteryVoltageTestState::onEntry()
{
    // Clear all the LEDs
    setGpioPinLow( pBatteryMotorLedGreenPin );
    setGpioPinLow( pBatteryMotorLedYellowPin );
    setGpioPinLow( pBatteryMotorLedRedPin );
    setGpioPinLow( pBatteryElectronicsLedGreenPin );
    setGpioPinLow( pBatteryElectronicsLedYellowPin );
    setGpioPinLow( pBatteryElectronicsLedRedPin );

    char tmp[17];
    strcpy_P( tmp, PSTR( "Motor Batt Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    strcpy_P( mLabel, PSTR( "V = " ) );

    getAndDisplayVoltage();
}


void MotorBatteryVoltageTestState::onExit()
{
    // Clear all the motor LEDs
    setGpioPinLow( pBatteryMotorLedGreenPin );
    setGpioPinLow( pBatteryMotorLedYellowPin );
    setGpioPinLow( pBatteryMotorLedRedPin );

    delete this;
}


bool MotorBatteryVoltageTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kEightSecondTimerEvent )
    {
        getAndDisplayVoltage();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    // Returning false short-circuits further event handling
    // which prevents the default handlers from adjusting LEDs
    return false;
}


void MotorBatteryVoltageTestState::getAndDisplayVoltage()
{
    int milliVolts = Battery::getMotorBatteryMilliVoltage();

    Battery::displayMotorBatteryStatusLed( milliVolts );

    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::print( mLabel );
    Display::setCursor( 1, 4 );
    Display::print( milliVolts );
}







/******************************************/


void CpuBatteryVoltageTestState::onEntry()
{
    // Clear all the LEDs
    setGpioPinLow( pBatteryMotorLedGreenPin );
    setGpioPinLow( pBatteryMotorLedYellowPin );
    setGpioPinLow( pBatteryMotorLedRedPin );
    setGpioPinLow( pBatteryElectronicsLedGreenPin );
    setGpioPinLow( pBatteryElectronicsLedYellowPin );
    setGpioPinLow( pBatteryElectronicsLedRedPin );

    char tmp[17];
    strcpy_P( tmp, PSTR( "CPU Batt Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    strcpy_P( mLabel, PSTR( "V = " ) );

    getAndDisplayVoltage();
}


void CpuBatteryVoltageTestState::onExit()
{
    // Clear all the electronics LEDs
    setGpioPinLow( pBatteryElectronicsLedGreenPin );
    setGpioPinLow( pBatteryElectronicsLedYellowPin );
    setGpioPinLow( pBatteryElectronicsLedRedPin );

    delete this;
}


bool CpuBatteryVoltageTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kEightSecondTimerEvent )
    {
        getAndDisplayVoltage();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    // Returning false short-circuits further event handling
    // which prevents the default handlers from adjusting LEDs
    return false;
}


void CpuBatteryVoltageTestState::getAndDisplayVoltage()
{
    int milliVolts = Battery::getCpuBatteryMilliVoltage();

    Battery::displayCpuBatteryStatusLed( milliVolts );

    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::print( mLabel );
    Display::setCursor( 1, 4 );
    Display::print( milliVolts );
}







/******************************************/


void AvailableMemoryTestState::onEntry()
{
    mDisplayFreeMemory = true;

    strcpy_P( mLabelUsed, PSTR( "Used Memory" ) );
    strcpy_P( mLabelFree, PSTR( "Free Memory" ) );

    getAndDisplayMemory();
}


bool AvailableMemoryTestState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new TestMenuState );
        }
        else
        {
            // Toggle display
            mDisplayFreeMemory = !mDisplayFreeMemory;
            getAndDisplayMemory();
        }
    }

    return true;
}


void AvailableMemoryTestState::getAndDisplayMemory()
{
    const unsigned int kMega2560SRam = 8192;

    if ( mDisplayFreeMemory )
    {
        Display::clear();
        Display::displayTopRow( mLabelFree );
        Display::setCursor( 1, 0 );
        Display::print( MemUtils::freeRam() );
    }
    else
    {
        Display::clear();
        Display::setCursor( 0, 0 );
        Display::print( mLabelUsed );
        Display::setCursor( 1, 0 );
        Display::print( kMega2560SRam - MemUtils::freeRam() );
    }
}





/**************************************************************/


void RangeScanTestState::onEntry()
{
    mIncrement = +10;
    mCurrentSlewAngle = 0;
    Radar::slew( mCurrentSlewAngle );

    char tmp[17];
    strcpy_P( tmp, PSTR( "Range Scan Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    strcpy_P( mLabelRng, PSTR( "Rng = " ) );

    // Allow time for the servo to slew
    CarrtCallback::yield( 500 );

    getAndDisplayRange();
}


void RangeScanTestState::onExit()
{
    Radar::slew( 0 );
}


bool RangeScanTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kEightSecondTimerEvent )
    {
        updateSlewAngle();
        Radar::slew( mCurrentSlewAngle );

        // Allow time for the servo to slew
        CarrtCallback::yield( 500 );

        getAndDisplayRange();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void RangeScanTestState::getAndDisplayRange()
{
    int rng = Radar::getDistanceInCm();
    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::print( mLabelRng );
    Display::setCursor( 1, 6 );
    Display::print( rng );
}


void RangeScanTestState::updateSlewAngle()
{
    if ( mIncrement > 0 )
    {
        // Moving right

        if ( mCurrentSlewAngle + mIncrement > 60 )
        {
            // Hit the right limit, reverse direction
            mIncrement *= -1;
        }

    }
    else
    {
        // Moving left

        if ( mCurrentSlewAngle + mIncrement < -60 )
        {
            // Hit the left limit, reverse direction
            mIncrement *= -1;
        }
    }

    mCurrentSlewAngle += mIncrement;
}






/**************************************************************/


void CompassTestState::onEntry()
{
    char tmp[17];
    strcpy_P( tmp, PSTR( "Compass Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    strcpy_P( mLabelHdg, PSTR( "Hdg = " ) );

    getAndDisplayCompassHeading();
}


bool CompassTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        getAndDisplayCompassHeading();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void CompassTestState::getAndDisplayCompassHeading()
{
    int heading = LSM303DLHC::getHeading();

    Display::displayBottomRow( mLabelHdg );
    Display::setCursor( 1, 6 );
    Display::print( heading );
}








/**************************************************************/


void AccelerometerTestState::onEntry()
{
    Display::clear();
    char tmp[17];
    strcpy_P( tmp, PSTR( "Accelerometer" ) );
    Display::displayTopRow( tmp );
    strcpy_P( tmp, PSTR( "Nulling out..." ) );
    Display::displayBottomRow( tmp );

    strcpy_P( mLabelX, PSTR( "X axis Gs Raw" ) );
    strcpy_P( mLabelY, PSTR( "Y axis Gs Raw" ) );
    strcpy_P( mLabelZ, PSTR( "Z axis Gs Raw" ) );

    strcpy_P( mLabelX0, PSTR( "X axis Gs Nulled" ) );
    strcpy_P( mLabelY0, PSTR( "Y axis Gs Nulled" ) );
    strcpy_P( mLabelZ0, PSTR( "Z axis Gs Nulled" ) );

    const uint8_t kNbrSamples = 32;
    mA0 = LSM303DLHC::getAccelerationG();
    for ( uint8_t i = 1; i < kNbrSamples; ++i )
    {
        delayMilliseconds( 50 );
        mA0 += LSM303DLHC::getAccelerationG();
    }
    mA0 /= kNbrSamples;

    mNulled = false;
    mAxis = 0;
    getAndDisplayAcceleration();
}


bool AccelerometerTestState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        getAndDisplayAcceleration();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new TestMenuState );
        }
        else if ( button & Keypad::kButton_Left )
        {
            // Switch axis
            --mAxis;
            mAxis += 3;
            mAxis %= 3;
            getAndDisplayAcceleration();
        }
        else if ( button & Keypad::kButton_Right )
        {
            // Switch axis
            ++mAxis;
            mAxis %= 3;
            getAndDisplayAcceleration();
        }
        else if ( button & Keypad::kButton_Down || button & Keypad::kButton_Up )
        {
            // Switch from Nulled to Raw
            mNulled = !mNulled;
            getAndDisplayAcceleration();
        }
    }

    return true;
}


void AccelerometerTestState::getAndDisplayAcceleration()
{
    Vector3Float a = LSM303DLHC::getAccelerationG();

    char* xLabel = mLabelX;
    char* yLabel = mLabelY;
    char* zLabel = mLabelZ;

    if ( mNulled )
    {
        a -= mA0;

        xLabel = mLabelX0;
        yLabel = mLabelY0;
        zLabel = mLabelZ0;
    }

    Display::clear();
    Display::setCursor( 0, 0 );

    switch ( mAxis )
    {
        case 0:
            Display::print( xLabel );
            Display::setCursor( 1, 0 );
            Display::print( a.x );
            break;

        case 1:
            Display::print( yLabel );
            Display::setCursor( 1, 0 );
            Display::print( a.y );
            break;

        case 2:
            Display::print( zLabel );
            Display::setCursor( 1, 0 );
            Display::print( a.z );
            break;
    }
}










/**************************************************************/


void GyroscopeTestState::onEntry()
{
    Display::clear();
    char tmp[17];
    strcpy_P( tmp, PSTR( "Gyroscope" ) );
    Display::displayTopRow( tmp );
    strcpy_P( tmp, PSTR( "Nulling out..." ) );
    Display::displayBottomRow( tmp );

    strcpy_P( mLabelX, PSTR( "X axis D/s Raw" ) );
    strcpy_P( mLabelY, PSTR( "Y axis D/s Raw" ) );
    strcpy_P( mLabelZ, PSTR( "Z axis D/s Raw" ) );

    strcpy_P( mLabelX0, PSTR( "X axis D/s Null" ) );
    strcpy_P( mLabelY0, PSTR( "Y axis D/s Null" ) );
    strcpy_P( mLabelZ0, PSTR( "Z axis D/s Null" ) );

    const uint8_t kNbrSamples = 32;
    mR0 = L3GD20::getAngularRatesDegreesPerSecond();
    for ( uint8_t i = 1; i < kNbrSamples; ++i )
    {
        delayMilliseconds( 50 );
        mR0 += L3GD20::getAngularRatesDegreesPerSecond();
    }
    mR0 /= kNbrSamples;

    mNulled = false;
    mAxis = 0;
    getAndDisplayAngularRates();
}


bool GyroscopeTestState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        getAndDisplayAngularRates();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new TestMenuState );
        }
        else if ( button & Keypad::kButton_Left )
        {
            // Switch axis
            --mAxis;
            mAxis += 3;
            mAxis %= 3;
            getAndDisplayAngularRates();
        }
        else if ( button & Keypad::kButton_Right )
        {
            // Switch axis
            ++mAxis;
            mAxis %= 3;
            getAndDisplayAngularRates();
        }
        else if ( button & Keypad::kButton_Down || button & Keypad::kButton_Up )
        {
            // Switch from Nulled to Raw
            mNulled = !mNulled;
            getAndDisplayAngularRates();
        }
    }

    return true;
}


void GyroscopeTestState::getAndDisplayAngularRates()
{
    Vector3Float r = L3GD20::getAngularRatesDegreesPerSecond();

    char* xLabel = mLabelX;
    char* yLabel = mLabelY;
    char* zLabel = mLabelZ;

    if ( mNulled )
    {
        r -= mR0;

        xLabel = mLabelX0;
        yLabel = mLabelY0;
        zLabel = mLabelZ0;
    }

    Display::clear();
    Display::setCursor( 0, 0 );

    switch ( mAxis )
    {
        case 0:
            Display::print( xLabel );
            Display::setCursor( 1, 0 );
            Display::print( r.x );
            break;

        case 1:
            Display::print( yLabel );
            Display::setCursor( 1, 0 );
            Display::print( r.y );
            break;

        case 2:
            Display::print( zLabel );
            Display::setCursor( 1, 0 );
            Display::print( r.z );
            break;
    }
}









/******************************************/


void MotorFwdRevTestState::onEntry()
{
    strcpy_P( mLabelFwd, PSTR( "Forward" ) );
    strcpy_P( mLabelRev, PSTR( "Reverse" ) );
    strcpy_P( mLabelStop, PSTR( "Stopped" ) );

    char tmp[17];
    strcpy_P( tmp, PSTR( "Fwd/Rev Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    mDriveStatus = kStopped;
    mElapsedSeconds = 0;
}


void MotorFwdRevTestState::onExit()
{
    Motors::stop();

    delete this;
}


bool MotorFwdRevTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        ++mElapsedSeconds;
        mElapsedSeconds %= 3;
        if ( !mElapsedSeconds  )
        {
            updateDriveStatus();

            if ( mDriveStatus == kStopped || mDriveStatus == kPause )
            {
                Beep::beep();
                Display::displayBottomRow( mLabelStop );
                Motors::stop();
            }

            if ( mDriveStatus == kFwd )
            {
                Beep::beep();
                Display::displayBottomRow( mLabelFwd );
                Motors::goForward();
            }

            if ( mDriveStatus == kRev )
            {
                Beep::beep();
                Display::displayBottomRow( mLabelRev);
                Motors::goBackward();
            }
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();
        Display::displayBottomRow( mLabelStop );
        mDriveStatus = kStopped;
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void MotorFwdRevTestState::updateDriveStatus()
{
    if ( mDriveStatus == kStopped )
    {
        mDriveStatus == kFwd;
    }
    else if ( mDriveStatus == kFwd )
    {
        mDriveStatus == kPause;
    }
    else if ( mDriveStatus == kPause )
    {
        mDriveStatus = kRev;
    }
    else if ( mDriveStatus == kRev )
    {
        mDriveStatus == kStopped;
    }
}









/******************************************/


void MotorLeftRightTestState::onEntry()
{
    strcpy_P( mLabelLeft, PSTR( "Left" ) );
    strcpy_P( mLabelRight, PSTR( "Right" ) );
    strcpy_P( mLabelStop, PSTR( "Stopped" ) );

    char tmp[17];
    strcpy_P( tmp, PSTR( "Left/Right Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );

    mDriveStatus = kStopped;
    mElapsedSeconds = 0;
}


void MotorLeftRightTestState::onExit()
{
    Motors::stop();

    delete this;
}


bool MotorLeftRightTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        ++mElapsedSeconds;
        mElapsedSeconds %= 3;
        if ( !mElapsedSeconds  )
        {
            updateDriveStatus();

            if ( mDriveStatus == kStopped || mDriveStatus == kPause )
            {
                Beep::beep();
                Display::displayBottomRow( mLabelStop );
                Motors::stop();
            }

            if ( mDriveStatus == kLeft )
            {
                Beep::beep();
                Display::displayBottomRow( mLabelLeft );
                Motors::rotateLeft();
            }

            if ( mDriveStatus == kRight )
            {
                Beep::beep();
                Display::displayBottomRow( mLabelRight);
                Motors::rotateRight();
            }
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();
        Display::displayBottomRow( mLabelStop );
        mDriveStatus = kStopped;
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void MotorLeftRightTestState::updateDriveStatus()
{
    if ( mDriveStatus == kStopped )
    {
        mDriveStatus == kLeft;
    }
    else if ( mDriveStatus == kLeft )
    {
        mDriveStatus == kPause;
    }
    else if ( mDriveStatus == kPause )
    {
        mDriveStatus = kRight;
    }
    else if ( mDriveStatus == kRight )
    {
        mDriveStatus == kStopped;
    }
}





/******************************************/


void ErrorTestState::onEntry()
{
    mCount = 5;

    char tmp[17];
    strcpy_P( tmp, PSTR( "Error Event Test" ) );
    Display::clear();
    Display::displayTopRow( tmp );
    Display::setCursor( 1, 0 );
    Display::print( mCount );
}


bool ErrorTestState::onEvent( uint8_t event, int16_t param )
{
    if ( mCount && event == EventManager::kOneSecondTimerEvent )
    {
        Display::clearBottomRow();
        Display::setCursor( 1, 0 );
        Display::print( --mCount );

        if ( !mCount )
        {
            MainProcess::postErrorEvent( kTestError1 );
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}















