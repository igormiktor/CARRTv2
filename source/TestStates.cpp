/*
    TestStates.cpp - Test States for CARRT

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



#if CARRT_INCLUDE_TESTS_IN_BUILD


#include "TestStates.h"

#include <avr/pgmspace.h>

#include "AVRTools/SystemClock.h"
#include "AVRTools/MemUtils.h"

#include "CarrtCallback.h"
#include "CarrtPins.h"
#include "ErrorCodes.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "Navigator.h"
#include "TestMenuStates.h"

#include "Drivers/Battery.h"
#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/Keypad.h"
#include "Drivers/L3GD20.h"
#include "Drivers/Lidar.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/Motors.h"
#include "Drivers/Sonar.h"
#include "Drivers/TempSensor.h"


#include "Utils/DebuggingMacros.h"



/******************************************/


void Event1_4TestState::onEntry()
{
    mCount = 0;

    Display::clear();
    Display::displayTopRowP16( PSTR( "1/4s Event Test" ) );
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

    Display::clear();
    Display::displayTopRowP16( PSTR( "1s Event Test" ) );
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

    Display::clear();
    Display::displayTopRowP16( PSTR( "8s Event Test" ) );
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

    Display::clear();
    Display::displayTopRowP16( PSTR( "Beep Test" ) );

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

    Display::clear();
    Display::displayTopRowP16( PSTR( "Temp Sensor Test" ) );

    getAndDisplayTemp();

    DEBUG_PRINT_P( PSTR( "Temp C = " ) );
    DEBUG_PRINTLN( TempSensor::getTempC() );
    DEBUG_PRINT_P( PSTR( "Temp F = " ) );
    DEBUG_PRINTLN( TempSensor::getTempF() );
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
    Display::printP16( PSTR( "Temp = " ) );
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

    Display::clear();
    Display::displayTopRowP16( PSTR( "Batt LED Test" ) );

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

    Display::clear();
    Display::displayTopRowP16( PSTR( "Motor Batt Test" ) );

    getAndDisplayVoltage();

    DEBUG_PRINT_P( PSTR( "Motor Batt mV = " ) );
    DEBUG_PRINTLN( Battery::getMotorBatteryMilliVoltage() );
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
    Display::printP16( PSTR( "V = " ) );
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

    Display::clear();
    Display::displayTopRowP16( PSTR( "CPU Batt Test" ) );

    getAndDisplayVoltage();

    DEBUG_PRINT_P( PSTR( "CPU Batt mV = " ) );
    DEBUG_PRINTLN( Battery::getCpuBatteryMilliVoltage() );
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
    Display::printP16( PSTR( "V = " ) );
    Display::setCursor( 1, 4 );
    Display::print( milliVolts );
}







/******************************************/


namespace
{
    const PROGMEM char sLabelSramFree[]     = "Free SRAM";
    const PROGMEM char sLabelSramUsed[]     = "Used SRAM";
    const PROGMEM char sLabelStack[]        = "Free Stack";
    const PROGMEM char sLabelBytes[]        = "bytes";

    const unsigned int kMega2560SRam        = 8192;
};


void AvailableMemoryTestState::onEntry()
{
    mDisplaySRAM = true;
    mDisplayFreeMemory = true;

    getAndDisplayMemory();

    DEBUG_PRINT_P( PSTR( "SRAM Free (bytes) = " ) );
    DEBUG_PRINTLN( MemUtils::freeSRAM() );
    DEBUG_PRINT_P( PSTR( "SRAM Used (bytes) = " ) );
    DEBUG_PRINTLN( kMega2560SRam - MemUtils::freeSRAM() );
    DEBUG_PRINT_P( PSTR( "Stack Free (bytes) = " ) );
    DEBUG_PRINTLN( MemUtils::freeMemoryBetweenHeapAndStack() );
}


bool AvailableMemoryTestState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new TestMenuState );
        }
        else if ( button & Keypad::kButton_Left || button & Keypad::kButton_Right )
        {
            // Switch from SRAM to Stack
            mDisplaySRAM = !mDisplaySRAM;
            getAndDisplayMemory();
        }
        else if ( mDisplaySRAM && ( button & Keypad::kButton_Down || button & Keypad::kButton_Up ) )
        {
            // if mDisplaySRAM mode, toggle between free<->used display
            mDisplayFreeMemory = !mDisplayFreeMemory;
            getAndDisplayMemory();
        }
    }

    return true;
}




void AvailableMemoryTestState::getAndDisplayMemory()
{
    if ( mDisplaySRAM )
    {
        // Display SRAM available....
        if ( mDisplayFreeMemory )
        {
            Display::clear();
            Display::displayTopRowP16( sLabelSramFree );

            unsigned int memFree = MemUtils::freeSRAM();
            unsigned long memFreePct = static_cast<unsigned long>( memFree ) * 100;
            memFreePct /= kMega2560SRam;

            Display::setCursor( 1, 0 );
            Display::print( memFree );

            uint8_t n = 13;
            if ( memFreePct >= 100 )
            {
                n = 12;
            }
            if ( memFreePct < 10 )
            {
                n = 14;
            }
            Display::setCursor( 1, n );
            Display::print( memFreePct );
            Display::print( '%' );
        }
        else
        {
            Display::clear();
            Display::displayTopRowP16( sLabelSramUsed );

            unsigned int memUsed = kMega2560SRam - MemUtils::freeSRAM();
            unsigned long memUsedPct = static_cast<unsigned long>( memUsed ) * 100;
            memUsedPct /= kMega2560SRam;

            Display::setCursor( 1, 0 );
            Display::print( memUsed );

            uint8_t n = 13;
            if ( memUsedPct >= 100 )
            {
                n = 12;
            }
            if ( memUsedPct < 10 )
            {
                n = 14;
            }
            Display::setCursor( 1, n );
            Display::print( memUsedPct );
            Display::print( '%' );
        }
    }
    else
    {
        Display::clear();
        Display::displayTopRowP16( sLabelStack );

        unsigned int stackSpace = MemUtils::freeMemoryBetweenHeapAndStack();

        Display::setCursor( 1, 0 );
        Display::print( stackSpace );

        Display::setCursor( 1, 11 );
        Display::printP16( sLabelBytes );
    }
}





/**************************************************************/


void SonarTestState::onEntry()
{
    Display::clear();

    Display::displayTopRowP16( PSTR( "Sonar Test" ) );

    // Allow time for the servo to slew
    CarrtCallback::yieldMilliseconds( 500 );
}


bool SonarTestState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        getAndDisplayRange();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void SonarTestState::getAndDisplayRange()
{
    //  0123456789012345
    //  xxxx cm  xxxx in
    int rngCm = Sonar::getDistanceInCm();

    Display::clearBottomRow();
    Display::setCursor( 1, 5 );
    Display::printP16( PSTR( "cm" ) );
    Display::setCursor( 1, 14 );
    Display::printP16( PSTR( "in" ) );

    if ( rngCm == Sonar::kNoSonarEcho )
    {
        Display::setCursor( 1, 0 );
        Display::printP16( PSTR( "****" ) );
        Display::setCursor( 1, 9 );
        Display::printP16( PSTR( "****" ) );

    }
    else
    {
        Display::setCursor( 1, 0 );
        Display::print( rngCm );
        int rngIn = static_cast<int>( static_cast<float>( rngCm ) / 2.54 + 0.5 );
        Display::setCursor( 1, 9 );
        Display::print( rngIn );
    }
}






/**************************************************************/


void LidarTestState::onEntry()
{
    mCurrentSlewAngle = 0;
    Lidar::slew( mCurrentSlewAngle );

    Display::clear();

    displayBearing();

    // Allow time for the servo to slew
    CarrtCallback::yieldMilliseconds( 500 );

    DEBUG_PRINTLN_P( PSTR( "Lidar Test" ) );
    DEBUG_PRINTLN_P( PSTR( "Brg(R), Rng(cm)" ) );
}


void LidarTestState::onExit()
{
    Lidar::slew( 0 );

    delete this;

    DEBUG_PRINTLN_P( PSTR( "End" ) );
}


bool LidarTestState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Left )
        {
            --mCurrentSlewAngle;
            if ( mCurrentSlewAngle < -80 )
            {
                mCurrentSlewAngle = -80;
            }
            Lidar::slew( mCurrentSlewAngle );
            displayBearing();
        }
        else if ( button & Keypad::kButton_Right )
        {
            ++mCurrentSlewAngle;
            if ( mCurrentSlewAngle > 80 )
            {
                mCurrentSlewAngle = 80;
            }
            Lidar::slew( mCurrentSlewAngle );
            displayBearing();
        }
        else if ( button & Keypad::kButton_Down || button & Keypad::kButton_Up )
        {
            getAndDisplayRange();
        }
        else if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new TestMenuState );
        }
    }

    return true;
}


void LidarTestState::displayBearing()
{
    Display::clearTopRow();

    //0123456789012345
    //Lidar Brg  xxxx

    Display::displayTopRowP16( PSTR( "Lidar Brg" ) );
    Display::setCursor( 0, 11 );
    Display::print( mCurrentSlewAngle );
}


void LidarTestState::getAndDisplayRange()
{
    //  0123456789012345
    //  xxxx cm  xxxx in
    int rngCm;
    int err = Lidar::getDistanceInCm( &rngCm );

    Display::clearBottomRow();
    Display::setCursor( 1, 5 );
    Display::printP16( PSTR( "cm" ) );
    Display::setCursor( 1, 14 );
    Display::printP16( PSTR( "in" ) );

    if ( err )
    {
        Display::setCursor( 1, 0 );
        Display::printP16( PSTR( "****" ) );
        Display::setCursor( 1, 9 );
        Display::printP16( PSTR( "****" ) );

    }
    else
    {
        Display::setCursor( 1, 0 );
        Display::print( rngCm );
        int rngIn = static_cast<int>( static_cast<float>( rngCm ) / 2.54 + 0.5 );
        Display::setCursor( 1, 9 );
        Display::print( rngIn );
    }

    DEBUG_PRINT( mCurrentSlewAngle );
    DEBUG_PRINT_P( PSTR( ", " ) );
    DEBUG_PRINTLN( rngCm );
}






/**************************************************************/


void RangeScanTestState::onEntry()
{
    mElapsedSeconds = 0;
    mIncrement = +10;
    mCurrentSlewAngle = 0;
    Lidar::slew( mCurrentSlewAngle );

    Display::clear();
    Display::displayTopRowP16( PSTR( "Range Scan Test" ) );

    // Allow time for the servo to slew
    CarrtCallback::yieldMilliseconds( 500 );

    getAndDisplayRange();

    DEBUG_PRINTLN_P( PSTR( "Rng Scan Test" ) );
    DEBUG_PRINTLN_P( PSTR( "Brg(R), Rng(cm)" ) );
}


void RangeScanTestState::onExit()
{
    Lidar::slew( 0 );

    delete this;

    DEBUG_PRINTLN_P( PSTR( "End" ) );
}


bool RangeScanTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        ++mElapsedSeconds;
        // Slew every 4 seconds.
        if ( mElapsedSeconds % 4 )
        {
            updateSlewAngle();
            Lidar::slew( mCurrentSlewAngle );

            // Allow time for the servo to slew
            CarrtCallback::yieldMilliseconds( 500 );

            getAndDisplayRange();
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void RangeScanTestState::getAndDisplayRange()
{
    int rng;
    int err = Lidar::getDistanceInCm( &rng );

    Display::clearBottomRow();
    Display::setCursor( 1, 0 );
    Display::printP16( PSTR( "Rng = " ) );
    Display::setCursor( 1, 6 );
    if ( err )
    {
        Display::print( -1 );
    }
    else
    {
        Display::print( rng );
    }

    DEBUG_PRINT( mCurrentSlewAngle );
    DEBUG_PRINT_P( PSTR( ", " ) );
    DEBUG_PRINTLN( rng );
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
    Display::clear();
    Display::displayTopRowP16( PSTR( "Compass Test" ) );

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

    Display::displayBottomRowP16( PSTR( "Hdg = " ) );
    Display::setCursor( 1, 6 );
    Display::print( heading );
}








/**************************************************************/


namespace
{
    const PROGMEM char sLabelAX[]       = "X axis Gs Raw";
    const PROGMEM char sLabelAY[]       = "Y axis Gs Raw";
    const PROGMEM char sLabelAZ[]       = "Z axis Gs Raw";

    const PROGMEM char sLabelAX0[]      = "X axis Gs Nulled";
    const PROGMEM char sLabelAY0[]      = "Y axis Gs Nulled";
    const PROGMEM char sLabelAZ0[]      = "Z axis Gs Nulled";
};


void AccelerometerTestState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( PSTR( "Accelerometer" ) );
    Display::displayBottomRowP16( PSTR( "Nulling out..." ) );

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

    PGM_P xLabel = sLabelAX;
    PGM_P yLabel = sLabelAY;
    PGM_P zLabel = sLabelAZ;

    if ( mNulled )
    {
        a -= mA0;

        xLabel = sLabelAX0;
        yLabel = sLabelAY0;
        zLabel = sLabelAZ0;
    }

    Display::clear();
    Display::setCursor( 0, 0 );

    switch ( mAxis )
    {
        case 0:
            Display::printP16( xLabel );
            Display::setCursor( 1, 0 );
            Display::print( a.x );
            break;

        case 1:
            Display::printP16( yLabel );
            Display::setCursor( 1, 0 );
            Display::print( a.y );
            break;

        case 2:
            Display::printP16( zLabel );
            Display::setCursor( 1, 0 );
            Display::print( a.z );
            break;
    }
}










/**************************************************************/


namespace
{
    const PROGMEM char sLabelGX[]       = "X axis D/s Raw";
    const PROGMEM char sLabelGY[]       = "Y axis D/s Raw";
    const PROGMEM char sLabelGZ[]       = "Z axis D/s Raw";

    const PROGMEM char sLabelGX0[]      = "X axis D/s Nulled";
    const PROGMEM char sLabelGY0[]      = "Y axis D/s Nulled";
    const PROGMEM char sLabelGZ0[]      = "Z axis D/s Nulled";
};


void GyroscopeTestState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( PSTR( "Gyroscope" ) );
    Display::displayBottomRowP16( PSTR( "Nulling out..." ) );

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

    PGM_P xLabel = sLabelGX;
    PGM_P yLabel = sLabelGY;
    PGM_P zLabel = sLabelGZ;

    if ( mNulled )
    {
        r -= mR0;

        xLabel = sLabelGX0;
        yLabel = sLabelGY0;
        zLabel = sLabelGZ0;
    }

    Display::clear();
    Display::setCursor( 0, 0 );

    switch ( mAxis )
    {
        case 0:
            Display::printP16( xLabel );
            Display::setCursor( 1, 0 );
            Display::print( r.x );
            break;

        case 1:
            Display::printP16( yLabel );
            Display::setCursor( 1, 0 );
            Display::print( r.y );
            break;

        case 2:
            Display::printP16( zLabel );
            Display::setCursor( 1, 0 );
            Display::print( r.z );
            break;
    }
}









/******************************************/


namespace
{
    const PROGMEM char sLabelFwd[]      = "Forward";
    const PROGMEM char sLabelRev[]      = "Reverse";
    const PROGMEM char sLabelStop[]     = "Stopped";
};


void MotorFwdRevTestState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( PSTR( "Fwd/Rev Test" ) );

    mDriveStatus = kStopped;
    mElapsedSeconds = 0;
}


void MotorFwdRevTestState::onExit()
{
    Motors::stop();

    delete this;
}


void MotorFwdRevTestState::pause()
{
    // In all situtations stopping motors is the right thing to do
    Motors::stop();
}


void MotorFwdRevTestState::unpause()
{
    // Slight delay to let fingers get away from CARRT
    delayMilliseconds( 500 );
    
    // If moving, restart motors appropriately
    switch ( mDriveStatus )
    {
        case kFwd:
            Motors::goForward();
            break;

        case kRev:
            Motors::goBackward();
            break;
    }
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
                Display::displayBottomRowP16( sLabelStop );
                Motors::stop();
            }

            if ( mDriveStatus == kFwd )
            {
                Beep::beep();
                Display::displayBottomRowP16( sLabelFwd );
                Motors::goForward();
            }

            if ( mDriveStatus == kRev )
            {
                Beep::beep();
                Display::displayBottomRowP16( sLabelRev);
                Motors::goBackward();
            }
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();
        Display::displayBottomRowP16( sLabelStop );
        mDriveStatus = kStopped;
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void MotorFwdRevTestState::updateDriveStatus()
{
    if ( mDriveStatus == kStopped )
    {
        mDriveStatus = kFwd;
    }
    else if ( mDriveStatus == kFwd )
    {
        mDriveStatus = kPause;
    }
    else if ( mDriveStatus == kPause )
    {
        mDriveStatus = kRev;
    }
    else if ( mDriveStatus == kRev )
    {
        mDriveStatus = kStopped;
    }
}









/******************************************/


namespace
{
    const PROGMEM char sLabelLeft[]     = "Left";
    const PROGMEM char sLabelRight[]    = "Right";
};


void MotorLeftRightTestState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( PSTR( "Left/Right Test" ) );

    mDriveStatus = kStopped;
    mElapsedSeconds = 0;
}


void MotorLeftRightTestState::onExit()
{
    Motors::stop();

    delete this;
}


void MotorLeftRightTestState::pause()
{
    // In all situtations stopping motors is the right thing to do
    Motors::stop();
}


void MotorLeftRightTestState::unpause()
{
    // Slight delay to let fingers get away from CARRT
    delayMilliseconds( 500 );
    
    // If moving, restart motors appropriately
    switch ( mDriveStatus )
    {
        case kLeft:
            Motors::rotateLeft();
            break;

        case kRight:
            Motors::rotateRight();
            break;
    }
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
                Display::displayBottomRowP16( sLabelStop );
                Motors::stop();
            }

            if ( mDriveStatus == kLeft )
            {
                Beep::beep();
                Display::displayBottomRowP16( sLabelLeft );
                Motors::rotateLeft();
            }

            if ( mDriveStatus == kRight )
            {
                Beep::beep();
                Display::displayBottomRowP16( sLabelRight );
                Motors::rotateRight();
            }
        }
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        Motors::stop();
        Display::displayBottomRowP16( sLabelStop );
        mDriveStatus = kStopped;
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void MotorLeftRightTestState::updateDriveStatus()
{
    if ( mDriveStatus == kStopped )
    {
        mDriveStatus = kLeft;
    }
    else if ( mDriveStatus == kLeft )
    {
        mDriveStatus = kPause;
    }
    else if ( mDriveStatus == kPause )
    {
        mDriveStatus = kRight;
    }
    else if ( mDriveStatus == kRight )
    {
        mDriveStatus = kStopped;
    }
}





/******************************************/

namespace
{
    //                                             1234567890123456
    const PROGMEM char sLabelNavRotTest[]       = "Nav. Rotate Test";
    const PROGMEM char sLabelNavRotTestInstr[]  = "Rotate & Observe";
    const PROGMEM char sLabelCompData[]         = "C:";
    const PROGMEM char sLabelNavData[]          = "N:";
};


void NavigatorRotateTestState::onEntry()
{
    Display::clear();

    Display::displayTopRowP16( sLabelNavRotTest );
    Display::displayBottomRowP16( sLabelNavRotTestInstr );

    CarrtCallback::yieldMilliseconds( 3000 );

    Navigator::movingTurning();

    displayNavInfo();
}


void NavigatorRotateTestState::onExit()
{
    Navigator::stopped();
    Navigator::reset();

    delete this;
}


bool NavigatorRotateTestState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kOneSecondTimerEvent )
    {
        displayNavInfo();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new TestMenuState );
    }

    return true;
}


void NavigatorRotateTestState::displayNavInfo()
{
    Display::clearBottomRow();

    // 0123456789012345
    // C: xxx  N: xxx

    int compassHdg = LSM303DLHC::getHeading();
    int navigatorHdg = static_cast<int>( Navigator::getCurrentHeading() + 0.5 );

    Display::setCursor( 1, 0 );
    Display::printP16( sLabelCompData );
    Display::setCursor( 1, 3 );
    Display::print( compassHdg );
    Display::setCursor( 1, 8 );
    Display::printP16( sLabelNavData );
    Display::setCursor( 1, 11 );
    Display::print( navigatorHdg );
}





/******************************************/

namespace
{

    const uint8_t kSecondsToDriveEachTime       = 5;

    //                                             1234567890123456
    const PROGMEM char sLabelNavDrvTest[]       = "Nav. Drive Test";
    const PROGMEM char sLabelNavInit[]          = "Initializing";
    const PROGMEM char sLabelNavDist[]          = "Dist";
    const PROGMEM char sLabelCm[]               = "cm";
};


void NavigatorDriveTestState::onEntry()
{
    Display::clear();

    Display::displayTopRowP16( sLabelNavDrvTest );
    Display::displayBottomRowP16( sLabelNavInit );

    displayNavInfo();

    CarrtCallback::yieldMilliseconds( 3000 );

    mNextDirection = kFwd;
    mStatus = kReadyToGo;
}


void NavigatorDriveTestState::onExit()
{
    Motors::stop();
    Navigator::stopped();
    Navigator::reset();

    delete this;
}


void NavigatorDriveTestState::pause()
{
    // In all situtations stopping motors is the right thing to do
    Motors::stop();
}


void NavigatorDriveTestState::unpause()
{
    // Slight delay to let fingers get away from CARRT
    delayMilliseconds( 500 );
    
    // If moving, restart motors appropriately
    if ( mStatus == kGoing )
    {
        switch ( mNextDirection )
        {
            case kFwd:
                // If next direction is forward, we were paused going reverse
                Motors::goBackward();
                break;

            case kRev:
                // If next direction is reverse, we were paused going forward
                Motors::goForward();
                break;
        }
    }
}


bool NavigatorDriveTestState::onEvent( uint8_t event, int16_t button )
{

    if ( event == EventManager::kOneSecondTimerEvent )
    {
        // Note that we start and stop driving on full second events

        switch ( mStatus )
        {
            case kReadyToGo:
                Navigator::movingStraight();
                if ( mNextDirection == kFwd )
                {
                    Motors::goForward();
                    mNextDirection = kRev;
                }
                else
                {
                    Motors::goBackward();
                    mNextDirection = kFwd;
                }
                mStatus = kGoing;
                // Only start the clock when we actually start driving
                mDriveTimeSecs = kSecondsToDriveEachTime;
                break;

            case kGoing:
                if ( !--mDriveTimeSecs )
                {
                    // Times up; stop driving
                    Motors::stop();
                    Navigator::stopped();
                    mStatus = kDisplaying;
                }
                break;

            default:
                break;
        }

        displayNavInfo();
    }
    else if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            MainProcess::changeState( new TestMenuState );
        }
        else if ( mStatus == kDisplaying )
        {
            // Any other button hit in displaying mode means ready to go
            mStatus = kReadyToGo;
            // Little pause for user to get out of the way
            CarrtCallback::yieldMilliseconds( 2000 );
        }
    }

    return true;
}


void NavigatorDriveTestState::displayNavInfo()
{
    Display::clear();

    // 0123456789012345
    // N sxxxx W sxxxx

    Vector2Float pos = Navigator::getCurrentPositionCm();

    Display::setCursor( 0, 0 );
    Display::print( 'N' );
    Display::setCursor( 0, 2 );
    Display::print( static_cast<int>( pos.x ) );
    Display::setCursor( 0, 8 );
    Display::print( 'W' );
    Display::setCursor( 0, 10 );
    Display::print( static_cast<int>( pos.y ) );

    // 0123456789012345
    // Dist  xxxxx  cm

    Display::setCursor( 1, 0 );
    Display::printP16( sLabelNavDist );
    Display::setCursor( 1, 6 );
    Display::print( static_cast<int>( norm( pos ) ) );
    Display::setCursor( 1, 13 );
    Display::printP16( sLabelCm );
}





/******************************************/


void ErrorTestState::onEntry()
{
    mCount = 5;

    Display::clear();
    Display::displayTopRowP16( PSTR( "Error Event Test" ) );
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






#endif  // CARRT_INCLUDE_TESTS_IN_BUILD
