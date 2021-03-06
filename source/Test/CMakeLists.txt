
cmake_minimum_required( VERSION 2.8 )



set( AVR_UPLOADTOOL avrdude )
set( AVR_PROGRAMMER wiring )
set( AVR_UPLOADTOOL_PORT /dev/CARRT )

set( AVR_MCU atmega2560 )
set( AVR_MCU_SPEED 16000000UL )

set( CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.cmake/avr-gcc-toolchain.cmake )

project( TestCARRT )

set( CMAKE_BUILD_TYPE Release )

enable_language( ASM )

include_directories( ".." )


set( DriverSrcs
        ../Drivers/Battery.cpp
        ../Drivers/Beep.cpp
        ../Drivers/DisplayAndKeypad.cpp
        ../Drivers/L3GD20.cpp
        ../Drivers/Lidar.cpp
        ../Drivers/LSM303DLHC.cpp
        ../Drivers/Motors.cpp
        ../Drivers/Sonar.cpp
        ../Drivers/Servo.cpp
        ../Drivers/TempSensor.cpp
    )


set( CarrtSrcs
        ../ErrorUnrecoverable.cpp
        ../EventClock.cpp
        ../EventManager.cpp
        ../Navigator.cpp
        ../NavigationMap.cpp
        ../PathSearch/Path.cpp
        ../PathSearch/PathFinder.cpp
        ../PathSearch/PathFinderMap.cpp
        ../PathSearch/FrontierList.cpp
        ../PathSearch/ExploredList.cpp
    )


set( CarrtUtilsSrcs
        ../Utils/DebuggingSupport.cpp
    )


set( AvrUtilsSrcs
        ../AVRTools/Analog2Digital.cpp
        ../AVRTools/InitSystem.cpp
        ../AVRTools/MemUtils.cpp
        ../AVRTools/SystemClock.cpp
        ../AVRTools/Pwm.cpp
        ../AVRTools/RingBuffer.cpp
        ../AVRTools/USART0.cpp
#        ../AVRTools/USART1.cpp
#        ../AVRTools/USART2.cpp
#        ../AVRTools/USART3.cpp
#        ../AVRTools/USART0Minimal.cpp
        ../AVRTools/Writer.cpp
        ../AVRTools/Reader.cpp
        ../AVRTools/abi.cpp
        ../AVRTools/new.cpp
        ../AVRTools/I2cMaster.cpp
        ../AVRTools/I2cLcd.cpp
        ../AVRTools/SimpleDelays.S
        ../AVRTools/SPI.cpp
    )


add_definitions( -DCARRT_INCLUDE_NAVMAP_IN_BUILD=1 )
add_definitions( -DCARRT_NAVIGATE_USING_INERTIAL=1 )

add_avr_executable( BeepTest TestDrivers/BeepTest.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( SonarTest TestDrivers/SonarTest.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( LidarTest TestDrivers/LidarTest.cpp CarrtCallbackTesting.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )
set_target_properties( LidarTest.elf PROPERTIES COMPILE_DEFINITIONS "CARRT_INCLUDE_NAVMAP_IN_BUILD=1" )

add_avr_executable( BatteryTest TestDrivers/BatteryTest.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( TempSensorTest TestDrivers/TempSensorTest.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( L3GD20Test TestDrivers/L3GD20Test.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( LSM303DLHCTestA TestDrivers/LSM303DLHCTestA.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( LSM303DLHCTestM TestDrivers/LSM303DLHCTestM.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( MotorTestCollective TestDrivers/MotorTestCollective.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( DisplayTest TestDrivers/DisplayTest.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( KeypadTest TestDrivers/KeypadTest.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )



add_avr_executable( MenuTestPgm MenuPgmTest.cpp ${AvrUtilsSrcs} )

add_avr_executable( MenuTestSram MenuSramTest.cpp ${AvrUtilsSrcs} )


add_avr_executable( SonarTiming SonarTimingTest.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( SonarRangingTest SonarRangingTest.cpp CarrtCallbackTesting.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )
set_target_properties( SonarRangingTest.elf PROPERTIES COMPILE_DEFINITIONS
    "kCarrtNavigationMapGridSize=32;CARRT_ENABLE_NAVIGATION_MAP_DEBUG=1" )

add_avr_executable( LidarRangingTest LidarRangingTest.cpp CarrtCallbackTesting.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )
set_target_properties( LidarRangingTest.elf PROPERTIES COMPILE_DEFINITIONS
    "kCarrtNavigationMapGridSize=32;CARRT_ENABLE_NAVIGATION_MAP_DEBUG=1" )

add_avr_executable( PathFindingBasicTest PathFindingBasicTest.cpp DisplayMap.cpp CarrtCallbackTesting.cpp
    ${CarrtSrcs} ${DriverSrcs} ${CarrtUtilsSrcs} ${AvrUtilsSrcs} )
set_target_properties( PathFindingBasicTest.elf PROPERTIES COMPILE_DEFINITIONS
    "CARRT_ENABLE_DEBUG_SERIAL=1;CARRT_ENABLE_NAVIGATION_MAP_DEBUG=1;CARRT_ENABLE_AVR_PATHFINDER_DEBUG=1;CARRT_PFBT_USE_MAP2=1" )

add_avr_executable( PathFindingTest PathFindingTest.cpp DisplayMap.cpp CarrtCallbackTesting.cpp ${CarrtSrcs} ${DriverSrcs} ${CarrtUtilsSrcs} ${AvrUtilsSrcs} )
set_target_properties( PathFindingTest.elf PROPERTIES COMPILE_DEFINITIONS
    "CARRT_ENABLE_DEBUG_SERIAL=1;CARRT_ENABLE_NAVIGATION_MAP_DEBUG=1;CARRT_ENABLE_AVR_PATHFINDER_DEBUG=1" )

add_avr_executable( NavigatorTiming NavigatorTimingTest.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )


add_avr_executable( EventClockTest EventClockTest.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( MotorTestIndividual TestDrivers/MotorTestIndividual.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )
set_target_properties( MotorTestIndividual.elf PROPERTIES COMPILE_DEFINITIONS "CARRT_TEST_INDIVIDUAL_MOTORS=1" )

add_avr_executable( MapMakingTest SonarMapMakingTest.cpp CarrtCallbackTesting.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )
set_target_properties( MapMakingTest.elf PROPERTIES COMPILE_DEFINITIONS
    "kCarrtNavigationMapGridSize=32;CARRT_ENABLE_NAVIGATION_MAP_DEBUG=1" )

