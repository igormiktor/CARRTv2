
cmake_minimum_required( VERSION 2.8 )



set( AVR_UPLOADTOOL avrdude )
set( AVR_PROGRAMMER wiring )
set( AVR_UPLOADTOOL_PORT /dev/CARRT )
#set( AVR_UPLOADTOOL_PORT /dev/ArduinoMega01 )

set( AVR_MCU atmega2560 )
set( AVR_MCU_SPEED 16000000UL )

set( CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.cmake/avr-gcc-toolchain.cmake )

project( CARRTv2 )

set( CMAKE_BUILD_TYPE Release )

add_definitions( -DCARRT_VERSION="2.3.1" )

enable_language( ASM )

include_directories( "." )


set( CarrtSrcs
        CarrtCallback.cpp
        CarrtMain.cpp
        DriveProgram.cpp
        ErrorState.cpp
        ErrorUnrecoverable.cpp
        EventClock.cpp
        EventManager.cpp
        GotoDriveMenuStates.cpp
        GotoDriveStates.cpp
        HelperStates.cpp
        MainProcess.cpp
        Menu.cpp
        MenuState.cpp
        Navigator.cpp
        NavigationMap.cpp
        ProgDriveStates.cpp
        ProgDriveMenuStates.cpp
        State.cpp
        TestMenuStates.cpp
        TestStates.cpp
        WelcomeMenuStates.cpp
        PathSearch/Path.cpp
        PathSearch/PathFinder.cpp
        PathSearch/PathFinderMap.cpp
        PathSearch/FrontierList.cpp
        PathSearch/ExploredList.cpp
    )


set( DriverSrcs
        Drivers/Battery.cpp
        Drivers/Beep.cpp
        Drivers/DisplayAndKeypad.cpp
        Drivers/DriveParam.cpp
        Drivers/L3GD20.cpp
        Drivers/Lidar.cpp
        Drivers/LSM303DLHC.cpp
        Drivers/Motors.cpp
        Drivers/Sonar.cpp
        Drivers/Servo.cpp
        Drivers/TempSensor.cpp
    )


set( UtilSrcs
        Utils/DebuggingSupport.cpp
    )


set( AvrUtilsSrcs
        AVRTools/Analog2Digital.cpp
        AVRTools/InitSystem.cpp
        AVRTools/MemUtils.cpp
        AVRTools/SystemClock.cpp
        AVRTools/Pwm.cpp
        AVRTools/RingBuffer.cpp
#        AVRTools/USART0.cpp
#        AVRTools/USART1.cpp
#        AVRTools/USART2.cpp
#        AVRTools/USART3.cpp
#        AVRTools/USART0Minimal.cpp
        AVRTools/Writer.cpp
        AVRTools/Reader.cpp
        AVRTools/abi.cpp
        AVRTools/new.cpp
        AVRTools/I2cMaster.cpp
        AVRTools/I2cLcd.cpp
        AVRTools/SimpleDelays.S
        AVRTools/SPI.cpp
    )


#add_definitions( -DCARRT_ENABLE_DEBUG_SERIAL=1 )
#add_definitions( -DCARRT_INCLUDE_TESTS_IN_BUILD=1 )
#add_definitions( -DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 )
#add_definitions( -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 )
#add_definitions( -DCARRT_INCLUDE_NAVMAP_IN_BUILD=1 )

add_avr_executable( Carrt ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( Carrt.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_TESTS_IN_BUILD=1 -DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_DEADRECKONING=1" )


add_avr_executable( CarrtNoTest ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( CarrtNoTest.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_DEADRECKONING=1" )



add_avr_executable( Carrt_IMU ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( Carrt_IMU.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_TESTS_IN_BUILD=1 -DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_INERTIAL=1" )


add_avr_executable( CarrtNoTest_IMU ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( CarrtNoTest_IMU.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_INERTIAL=1" )

