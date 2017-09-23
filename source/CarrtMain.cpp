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




#include <avr/pgmspace.h>

#include "AVRTools/Analog2Digital.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/InitSystem.h"
#include "AVRTools/MemUtils.h"
#include "AVRTools/SystemClock.h"

#include "DriveProgram.h"
#include "ErrorState.h"
#include "EventClock.h"
#include "MainProcess.h"
#include "Navigator.h"

#include "Drivers/Battery.h"
#include "Drivers/Beep.h"
#include "Drivers/Display.h"
#include "Drivers/L3GD20.h"
#include "Drivers/Lidar.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/Motors.h"

#include "Utils/DebuggingMacros.h"




namespace
{

    void initializeCPU();
    void initializeNetwork();
    void initializeDevices();
    void initializeIMU();
    void doResetActions();

    const PROGMEM char sMsgCarrtIs[]            = "CARRT is";
    const PROGMEM char sMsgCarrtImuIs[]         = "CARRT IMU is";
    const PROGMEM char sMsgInitializing[]       = "Initializing...";
};




int main()
{
    // Run/Reset loop
    while ( 1 )
    {
        initializeCPU();

        // Beep to announce we are spinning up
        Beep::initBeep();

        // Allow some time for capacitors to charge
        delayMilliseconds( 500 );

        initializeNetwork();
        initializeDevices();

        DEBUG_INIT_SERIAL_OUTPUT();
#if CARRT_ENABLE_DEBUG_SERIAL
        Display::displayBottomRowP16( PSTR( "Debug Enabled" ) );
#endif

        // Allow any 'power on' vibrations to dampen out before
        // starting inertial measurement unit
        delayMilliseconds( 2000 );
        initializeIMU();

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
        DriveProgram::init();
#endif

        // Create the error state (so we don't have to create it when out of memory; reused throughout)
        ErrorState errorState;

        MainProcess::init( &errorState );

        // Beep again to indicate ready to go
        Beep::readyChime();

        // Start the CARRT's internal clock (different from system clock)
        EventClock::init();

        // Everything important happens here...  Only ever return on a reset
        MainProcess::runEventLoop();

        // Only get here if resetting...
        doResetActions();
    }
}



namespace
{

    void doResetActions()
    {
        EventClock::stop();

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
        DriveProgram::purge();
#endif

        DEBUG_STOP_SERIAL_OUTPUT();

        MemUtils::resetHeap();
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
        Motors::init();

        // Initialize I2C network devices
        Display::init();
        Display::clear();
        Display::displayTopRowP16( sMsgCarrtIs );
        Display::displayBottomRowP16( sMsgInitializing );

        Lidar::init();
        LSM303DLHC::init();
        L3GD20::init();
    }



    void initializeIMU()
    {
        Display::displayTopRowP16( sMsgCarrtImuIs );
        Display::displayBottomRowP16( sMsgInitializing );

        Navigator::init();
    }

};
