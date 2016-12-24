/*
    CarrtMain.cpp - CARRT's main function.  Initializes and hands off to
    the Main Process.

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






#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/Analog2Digital.h"
#include "AVRTools/I2cMaster.h"

#if CARRT_DEBUG_SERIAL
#include "AVRTools/USART0.h"
#endif

#include "CarrtClock.h"
#include "MainProcess.h"
#include "ErrorState.h"

#include "Drivers/Battery.h"
#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/L3GD20.h"
#include "Drivers/Motors.h"
#include "Drivers/Radar.h"



void initializeCPU();
void initializeNetwork();
void initializeDevices();
void initializeIMU();



#if CARRT_DEBUG_SERIAL
   Serial0 gDebugSerial;
#endif



int main()
{
    initializeCPU();

    // Beep to announce we are spinning up
    Beep::initBeep();

    initializeNetwork();
    initializeDevices();

    // Allow any 'power on' vibrations to dampen out before
    // starting inertial measurement unit
    delayMilliseconds( 2000 );
    initializeIMU();


#if CARRT_DEBUG_SERIAL
    // Initialize serial output if we are debugging
    gDebugSerial.start( 115200 );
    Display::displayBottomRow( "Debug Enabled" );
    gDebugSerial.println( "Debug Enabled Output" );
#endif


    // Create the error state (so we don't have to create it when out of memory; reused throughout)
    ErrorState errorState;

    // Run/Reset loop
    while ( 1 )
    {
        MainProcess::init( &errorState );

        // Start the CARRT's internal clock (different from system clock)
        CarrtClock::init();

        // Everything else happens here...
        MainProcess::runEventLoop();

        // Only get here if resetting
        CarrtClock::stop();
        initializeIMU();
    }
}



void initializeCPU()
{
    initSystem();
    initSystemClock();
    initA2D( kA2dReference256V );
}


void initializeNetwork()
{
    I2cMaster::start();
}



void initializeDevices()
{
    // Initialize non-network devices first
    // (gives time for the I2C network to stabilize)
    Battery::initBatteryStatusDisplay();
    Radar::init();
    Motors::init();

    // Initialize I2C network devices
    Display::init();
    Display::clear();
    Display::displayTopRow( "CARRT is" );
    Display::displayBottomRow( "Initializing..." );

    LSM303DLHC::init();
    L3GD20::init();
}


void initializeIMU()
{
    Display::displayTopRow( "CARRT IMU is" );
    Display::displayBottomRow( "Initializing..." );
    // gNavigator.init();
}


