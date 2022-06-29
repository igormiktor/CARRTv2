/*
    Battery.cpp - Functions for checking bettery levels

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





#include "Battery.h"

#include <stdint.h>

#include "AVRTools/SystemClock.h"

#include "CarrtPins.h"
#include "CarrtCallback.h"

#include "AVRTools/Analog2Digital.h"



namespace
{

    // Thresholds are the cell millivolts times number of cells

    const int kMotorBatteryYellow    = 5 * 1150; // 1.15V/battery below this is yellow
    const int kMotorBatteryRed       = 5 * 1075; // 1.075V/battery below this is red
    const int kMotorBatteryShutdown  = 5 *  900; // 0.90V/battery threshold to shut down robot motors

    const int kCpuBatteryYellow      = 6 * 1150; // 1.15V/battery below this is yellow
    const int kCpuBatteryRed         = 6 * 1075; // 1.075V/battery below this is red
    const int kCpuBatteryShutdown    = 6 * 1025; // 6.15V total threshold to avoid CPU low-voltage reset



    // Return the voltge on the given pin in millivolts using internal 2.56 reference
    int getBatteryMilliVoltageViaReference( int8_t channel )
    {
        const long    kInternalVoltageReference   = 2560;       // Internal reference is 2.56 volts
        const float   kR1                         = 18.0;       // Voltage dividier resistors values
        const float   kR2                         = 2.2;
        const float   kResistorBridgeDivisor      = kR2/( kR1 + kR2 );
        const long    kMaxVoltage                 = static_cast<long>( kInternalVoltageReference / kResistorBridgeDivisor );

        int value = 0;
        for( int8_t i = 0; i < 8; i++)
        {
            value += readA2D( channel );
        }
        value  = value >> 3;            // divide by 8 (via bit shift) to get the average of 8 readings

        return value * kMaxVoltage / 1023;
    }



    // Flash the battery lights at startup to confirm all LEDs work and provide
    // visible status indicator of startup
    void doStartupFlash()
    {
        const int kFlashDelay = 200;    // delayMilliseconds

        for ( uint8_t i = 0; i < 3; ++i )
        {
            setGpioPinHigh( pBatteryMotorLedGreenPin );
            setGpioPinHigh( pBatteryMotorLedYellowPin );
            setGpioPinHigh( pBatteryMotorLedRedPin );
            setGpioPinHigh( pBatteryElectronicsLedGreenPin );
            setGpioPinHigh( pBatteryElectronicsLedYellowPin );
            setGpioPinHigh( pBatteryElectronicsLedRedPin );

            delayMilliseconds( kFlashDelay );

            setGpioPinLow( pBatteryMotorLedGreenPin );
            setGpioPinLow( pBatteryMotorLedYellowPin );
            setGpioPinLow( pBatteryMotorLedRedPin );
            setGpioPinLow( pBatteryElectronicsLedGreenPin );
            setGpioPinLow( pBatteryElectronicsLedYellowPin );
            setGpioPinLow( pBatteryElectronicsLedRedPin );

            delayMilliseconds( kFlashDelay );
        }
    }


}









void Battery::initBatteryStatusDisplay()
{
    // Set the charger detect pin to INPUT with PULLUP
    setGpioPinModeInputPullup( pBatteryMotorChargerConnectedPin );

    // Set the reference to internal 2.56V reference
    setA2DVoltageReference256V();

    // Allow ADC to settle by reading and discarding...
    readGpioPinAnalog( pTemperatureSensorPin );
    readGpioPinAnalog( pBatteryElectronicsVoltagePin );
    readGpioPinAnalog( pBatteryMotorVoltagePin );

    setGpioPinModeOutput( pBatteryMotorLedGreenPin );
    setGpioPinModeOutput( pBatteryMotorLedYellowPin );
    setGpioPinModeOutput( pBatteryMotorLedRedPin );
    setGpioPinModeOutput( pBatteryElectronicsLedGreenPin );
    setGpioPinModeOutput( pBatteryElectronicsLedYellowPin );
    setGpioPinModeOutput( pBatteryElectronicsLedRedPin );

    doStartupFlash();

    checkAndDisplayBatteryStatus();
}



// cppcheck-suppress unusedFunction
bool Battery::isChargerConnected()
{
    return readGpioPinDigital( pBatteryMotorChargerConnectedPin );
}



int Battery::getCpuBatteryMilliVoltage()
{
    return getBatteryMilliVoltageViaReference( getGpioADC( pBatteryElectronicsVoltagePin ) );
}



int Battery::getMotorBatteryMilliVoltage()
{
    return getBatteryMilliVoltageViaReference( getGpioADC( pBatteryMotorVoltagePin ) );
}






void Battery::displayMotorBatteryStatusLed( int milliVolts )
{
    // Turn them all off
    setGpioPinLow( pBatteryMotorLedGreenPin );
    setGpioPinLow( pBatteryMotorLedYellowPin );
    setGpioPinLow( pBatteryMotorLedRedPin );

    // Turn the right one on
    if ( milliVolts < kMotorBatteryRed )
    {
        setGpioPinHigh( pBatteryMotorLedRedPin );
    }
    else if ( milliVolts < kMotorBatteryYellow )
    {
        setGpioPinHigh( pBatteryMotorLedYellowPin );
    }
    else
    {
        setGpioPinHigh( pBatteryMotorLedGreenPin );
    }
}




void Battery::displayCpuBatteryStatusLed( int milliVolts )
{
    // Turn them all off
    setGpioPinLow( pBatteryElectronicsLedGreenPin );
    setGpioPinLow( pBatteryElectronicsLedYellowPin );
    setGpioPinLow( pBatteryElectronicsLedRedPin );

    // Turn the right one on
    if ( milliVolts < kCpuBatteryRed )
    {
        setGpioPinHigh( pBatteryElectronicsLedRedPin );
    }
    else if ( milliVolts < kCpuBatteryYellow )
    {
        setGpioPinHigh( pBatteryElectronicsLedYellowPin );
    }
    else
    {
        setGpioPinHigh( pBatteryElectronicsLedGreenPin );
    }
}







bool Battery::isMotorBatteryOkay( int milliVolts )
{
    return ( milliVolts >= kMotorBatteryShutdown );
}



bool Battery::isCpuBatteryOkay( int milliVolts )
{
    return ( milliVolts >= kCpuBatteryShutdown );
}




int Battery::checkAndDisplayBatteryStatus()
{
    int batteryStatus = kBatteriesOkay;

    if ( !CarrtCallback::isMoving() )
    {
        // Only check motor batteries while we are not moving
        // While motors are in operation experience significant voltage drops
        int motorMilliVolts = getMotorBatteryMilliVoltage();
        displayMotorBatteryStatusLed( motorMilliVolts );
        if ( !isMotorBatteryOkay( motorMilliVolts ) )
        {
            batteryStatus = kMotorBatteryCriticalLow;
        }
    }

    int cpuMilliVolts = getCpuBatteryMilliVoltage();
    displayCpuBatteryStatusLed( cpuMilliVolts );
    if ( !isCpuBatteryOkay( cpuMilliVolts ) )
    {
        batteryStatus = kCpuBatteryCriticalLow;
    }

    return batteryStatus;
}



