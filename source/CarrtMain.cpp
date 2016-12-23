
#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/Analog2Digital.h"
#include "AVRTools/I2cMaster.h"

#if DEBUG_SERIAL
#include "AVRTools/Serial0.h"
#endif

#include "CarrtClock.h"
#include "MainProcess.h"
#include "ErrorState.h"
#include "MenuStates.h"
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

#if DEBUG_SERIAL
    // Initialize serial output if we are debugging
    Serial0.begin( 38400 );
#endif


    // Create the error state (so we don't have to create it when out of memory; reused throughout)
    ErrorState errorState;
    State* welcome = new WelcomeState;

    MainProcess::init( welcome, &errorState );

    // Start the CARRT's internal clock (different from system clock)
    initCarrtClock();

    // Everything else happens here...
    MainProcess::runEventLoop();
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


