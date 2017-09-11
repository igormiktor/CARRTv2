
cmake_minimum_required( VERSION 2.8 )



set( AVR_UPLOADTOOL avrdude )
set( AVR_PROGRAMMER wiring )
set( AVR_UPLOADTOOL_PORT /dev/ArduinoMega02 )

set( AVR_MCU atmega2560 )
set( AVR_MCU_SPEED 16000000UL )

set( CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.cmake/avr-gcc-toolchain.cmake )

project( TestCARRTSubSystems )

set( CMAKE_BUILD_TYPE Release )

enable_language( ASM )

include_directories( "../.." )


set( DriverSrcs
        ../../Drivers/Battery.cpp
        ../../Drivers/Beep.cpp
        ../../Drivers/DisplayAndKeypad.cpp
        ../../Drivers/L3GD20.cpp
        ../../Drivers/Lidar.cpp
        ../../Drivers/LSM303DLHC.cpp
        ../../Drivers/Motors.cpp
        ../../Drivers/Radar.cpp
        ../../Drivers/Servo.cpp
        ../../Drivers/TempSensor.cpp
    )



set( AvrUtilsSrcs
        ../../AVRTools/Analog2Digital.cpp
        ../../AVRTools/InitSystem.cpp
        ../../AVRTools/MemUtils.cpp
        ../../AVRTools/SystemClock.cpp
        ../../AVRTools/Pwm.cpp
        ../../AVRTools/RingBuffer.cpp
        ../../AVRTools/USART0.cpp
#        ../../AVRTools/USART1.cpp
#        ../../AVRTools/USART2.cpp
#        ../../AVRTools/USART3.cpp
#        ../../AVRTools/USART0Minimal.cpp
        ../../AVRTools/Writer.cpp
        ../../AVRTools/Reader.cpp
        ../../AVRTools/abi.cpp
        ../../AVRTools/new.cpp
        ../../AVRTools/I2cMaster.cpp
        ../../AVRTools/I2cLcd.cpp
        ../../AVRTools/SimpleDelays.S
        ../../AVRTools/SPI.cpp
    )


add_definitions( -DCARRT_DISABLE_SERVO=1 )

add_avr_executable( LidarBasic LidarBasic.cpp ${AvrUtilsSrcs} )

add_avr_executable( LidarBasicTest LidarBasicTest.cpp ../CarrtCallbackTesting.cpp ${DriverSrcs} ${AvrUtilsSrcs} )
