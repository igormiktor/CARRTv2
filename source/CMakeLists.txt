
cmake_minimum_required( VERSION 2.8 )



set( AVR_UPLOADTOOL avrdude )
set( AVR_PROGRAMMER wiring )
set( AVR_UPLOADTOOL_PORT /dev/CARRT )

set( AVR_MCU atmega2560 )
set( AVR_MCU_SPEED 16000000UL )

set( CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.cmake/generic-gcc-avr.cmake )

project( CompileCheck )

set( CMAKE_BUILD_TYPE Release )

enable_language( ASM )

include_directories( "." )


set( CarrtSrcs
        CarrtCallback.cpp
        CarrtMain.cpp
        ErrorState.cpp
        EventClock.cpp
        EventManager.cpp
        MainProcess.cpp
        Menu.cpp
        MenuStates.cpp
        State.cpp
        TestStates.cpp
    )


set( DriverSrcs
        Drivers/Battery.cpp
        Drivers/Beep.cpp
        Drivers/DisplayAndKeypad.cpp
        Drivers/L3GD20.cpp
        Drivers/LSM303DLHC.cpp
        Drivers/Motors.cpp
        Drivers/Radar.cpp
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
add_definitions( -DCARRT_INCLUDE_TESTS_IN_BUILD=1 )
add_definitions( -DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 )

add_avr_executable( CarrtDev ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
