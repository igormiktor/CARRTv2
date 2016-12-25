
cmake_minimum_required( VERSION 2.8 )



set( AVR_UPLOADTOOL avrdude )
set( AVR_PROGRAMMER wiring )
set( AVR_UPLOADTOOL_PORT /dev/CARRT )

set( AVR_MCU atmega2560 )
set( AVR_MCU_SPEED 16000000UL )

#set( CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.cmake/generic-gcc-avr.cmake )
set( CMAKE_TOOLCHAIN_FILE ../../generic-gcc-avr.cmake )

project( TestCARRT )

set( CMAKE_BUILD_TYPE Release )

enable_language( ASM )

include_directories( ".." )


set( DriverSrcs
        ../Drivers/Battery.cpp
        ../Drivers/Beep.cpp
        ../Drivers/DisplayAndKeypad.cpp
        ../Drivers/L3GD20.cpp
        ../Drivers/LSM303DLHC.cpp
        ../Drivers/Motors.cpp
        ../Drivers/Radar.cpp
        ../Drivers/Servo.cpp
        ../Drivers/TempSensor.cpp
    )


set( CarrtSrcs
        ../EventClock.cpp
        ../EventManager.cpp
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


add_avr_executable( BeepTest TestDrivers/BeepTest.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

add_avr_executable( RadarTest TestDrivers/RadarTest.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )

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


add_avr_executable( EventClockTest EventClockTest.cpp ${CarrtSrcs} ${DriverSrcs} ${AvrUtilsSrcs} )

add_definitions( -DCARRT_TEST_INDIVIDUAL_MOTORS=1 )
add_avr_executable( MotorTestIndividual TestDrivers/MotorTestIndividual.cpp CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )
# target_compile_definitions( MotorTestIndividual PRIVATE CARRT_TEST_INDIVIDUAL_MOTORS=1 )

