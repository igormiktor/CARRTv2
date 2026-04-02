# CMakeLists.txt - cmake build file for CARRT

# Copyright (c) 2026 Igor Mikolic-Torreira.  All right reserved.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


cmake_minimum_required( VERSION 3.22 )

set( AVR_UPLOADTOOL avrdude )
set( AVR_PROGRAMMER wiring )
set( AVR_UPLOADTOOL_PORT /dev/CARRT )
#set( AVR_UPLOADTOOL_PORT /dev/ArduinoMega01 )

set( AVR_MCU atmega2560 )
set( AVR_MCU_SPEED 16000000UL )

set( CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.cmake/avr-gcc-toolchain.cmake )


project( CARRTv2 
    VERSION 2.3.3 
    DESCRIPTION "Executable for the CARRTv2 on AVR ATMega2560"
    LANGUAGES ASM C CXX 
)

set( VERSION_MAJOR 2 )
set( VERSION_MINOR 3 )
set( VERSION_REVISION 3 )

set( BUILD_DEBUG_VERSIONS ON )

message( "** Building with debugging is " ${BUILD_DEBUG_VERSIONS} )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )


# Various debugging options

option( 
    CARRT_ENABLE_EVENTMANAGER_DEBUG
    "Enable EventManager Debugging.  Default: OFF. Values: { OFF, ON }."
    OFF
)
option( 
    CARRT_ENABLE_GOTO_DEBUG
    "Enable Go-To drive Debugging.  Default: OFF. Values: { OFF, ON }."
    OFF
)
option( 
    CARRT_ENABLE_NAVIGATION_MAP_DEBUG
    "Enable Navigation Map Debugging.  Default: OFF. Values: { OFF, ON }."
    OFF
)
option( 
    CARRT_ENABLE_NAVIGATOR_DEBUG
    "Enable Navigator Debugging.  Default: OFF. Values: { OFF, ON }."
    OFF
)
option( 
    CARRT_ENABLE_AVR_PATHFINDER_DEBUG
    "Enable PathFinder Algorithm Debugging.  Default: OFF. Values: { OFF, ON }."
    OFF
)


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
        AVRTools/abi.cpp
        AVRTools/Analog2Digital.cpp       
        AVRTools/I2cLcd.cpp
        AVRTools/I2cMaster.cpp
        AVRTools/InitSystem.cpp
        AVRTools/MemUtils.cpp
        AVRTools/new.cpp
        AVRTools/Pwm.cpp
        AVRTools/SimpleDelays.S
        AVRTools/SPI.cpp
        AVRTools/SystemClock.cpp
        AVRTools/Reader.cpp
        AVRTools/RingBuffer.cpp
        AVRTools/Writer.cpp
    )

set( ArvUtilsDbgSrcs
       AVRTools/USART0.cpp
    )

if( BUILD_DEBUG_VERSIONS )
    add_definitions( -DCARRT_ENABLE_DEBUG_SERIAL=1 )
    set( AvrUtilsSrcs ${AvrUtilsSrcs} AVRTools/USART0.cpp )
endif()


set( CARRT_VERSION_STR "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}" )

# Set up automated extraction of git HEAD info

add_custom_target( GitHeadInfo
  COMMAND "${CMAKE_COMMAND}"
  "-D" "SRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
  "-D" "BIN_DIR=${CMAKE_CURRENT_BINARY_DIR}"
  "-P" "${PROJECT_SOURCE_DIR}/cmake/version.cmake"
  COMMENT "Generating git version file"
)

# Note AVRTools sources don't depend on any of the defines used below for executables
# So convenient to build into their own library for re-use across executables.
add_avr_library( AvrUtilsLib ${AvrUtilsSrcs} )
# Uncomment next line to always optimize AvrUtilsLib
#target_compile_options( AvrUtilsLib PRIVATE "-O3" )

# The 4 versions of the executable follow

add_avr_executable( Carrt ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} )
avr_target_link_libraries( Carrt AvrUtilsLib )
target_compile_definitions( Carrt.elf PRIVATE
    CARRT_INCLUDE_TESTS_IN_BUILD=1
    CARRT_INCLUDE_PROGDRIVE_IN_BUILD=1
    CARRT_INCLUDE_GOTODRIVE_IN_BUILD=1
    CARRT_NAVIGATE_USING_INERTIAL=0
    CARRT_NAVIGATE_USING_DEADRECKONING=1 
    
    CARRT_VERSION_MAJOR=${VERSION_MAJOR} 
    CARRT_VERSION_MINOR=${VERSION_MINOR} 
    CARRT_VERSION_REVISION=${VERSION_REVISION} 
    CARRT_VERSION_STR="${CARRT_VERSION_STR}"

    CARRT_ENABLE_EVENTMANAGER_DEBUG=$<BOOL:${CARRT_ENABLE_EVENTMANAGER_DEBUG}>
    CARRT_ENABLE_GOTO_DEBUG=$<BOOL:${CARRT_ENABLE_GOTO_DEBUG}> 
    CARRT_ENABLE_NAVIGATION_MAP_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATION_MAP_DEBUG}>
    CARRT_ENABLE_NAVIGATOR_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATOR_DEBUG}>
    CARRT_ENABLE_AVR_PATHFINDER_DEBUG=$<BOOL:${CARRT_ENABLE_AVR_PATHFINDER_DEBUG}>
)
add_dependencies( Carrt.elf GitHeadInfo )
target_include_directories( Carrt.elf PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR} )


add_avr_executable( CarrtNoTest ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} )
avr_target_link_libraries( CarrtNoTest AvrUtilsLib )
target_compile_definitions( CarrtNoTest.elf PRIVATE
    CARRT_INCLUDE_TESTS_IN_BUILD=0
    CARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 
    CARRT_INCLUDE_GOTODRIVE_IN_BUILD=1
    CARRT_NAVIGATE_USING_INERTIAL=0
    CARRT_NAVIGATE_USING_DEADRECKONING=1

    CARRT_VERSION_MAJOR=${VERSION_MAJOR} 
    CARRT_VERSION_MINOR=${VERSION_MINOR} 
    CARRT_VERSION_REVISION=${VERSION_REVISION} 
    CARRT_VERSION_STR="${CARRT_VERSION_STR}"

    CARRT_ENABLE_EVENTMANAGER_DEBUG=$<BOOL:${CARRT_ENABLE_EVENTMANAGER_DEBUG}>
    CARRT_ENABLE_GOTO_DEBUG=$<BOOL:${CARRT_ENABLE_GOTO_DEBUG}> 
    CARRT_ENABLE_NAVIGATION_MAP_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATION_MAP_DEBUG}>
    CARRT_ENABLE_NAVIGATOR_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATOR_DEBUG}>
    CARRT_ENABLE_AVR_PATHFINDER_DEBUG=$<BOOL:${CARRT_ENABLE_AVR_PATHFINDER_DEBUG}>
)
add_dependencies( CarrtNoTest.elf  GitHeadInfo )
target_include_directories( CarrtNoTest.elf PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR} )


add_avr_executable( Carrt_IMU ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} )
avr_target_link_libraries( Carrt_IMU AvrUtilsLib )
target_compile_definitions( Carrt_IMU.elf PRIVATE
    CARRT_INCLUDE_TESTS_IN_BUILD=1
    CARRT_INCLUDE_PROGDRIVE_IN_BUILD=1
    CARRT_INCLUDE_GOTODRIVE_IN_BUILD=1
    CARRT_NAVIGATE_USING_INERTIAL=1
    CARRT_NAVIGATE_USING_DEADRECKONING=0

    CARRT_VERSION_MAJOR=${VERSION_MAJOR} 
    CARRT_VERSION_MINOR=${VERSION_MINOR} 
    CARRT_VERSION_REVISION=${VERSION_REVISION} 
    CARRT_VERSION_STR="${CARRT_VERSION_STR}"

    CARRT_ENABLE_EVENTMANAGER_DEBUG=$<BOOL:${CARRT_ENABLE_EVENTMANAGER_DEBUG}>
    CARRT_ENABLE_GOTO_DEBUG=$<BOOL:${CARRT_ENABLE_GOTO_DEBUG}> 
    CARRT_ENABLE_NAVIGATION_MAP_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATION_MAP_DEBUG}>
    CARRT_ENABLE_NAVIGATOR_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATOR_DEBUG}>
    CARRT_ENABLE_AVR_PATHFINDER_DEBUG=$<BOOL:${CARRT_ENABLE_AVR_PATHFINDER_DEBUG}>
)
add_dependencies( Carrt_IMU.elf  GitHeadInfo )
target_include_directories( Carrt_IMU.elf PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR} )


add_avr_executable( CarrtNoTest_IMU ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} )
avr_target_link_libraries( CarrtNoTest_IMU AvrUtilsLib )
target_compile_definitions( CarrtNoTest_IMU.elf PUBLIC
    CARRT_INCLUDE_TESTS_IN_BUILD=0
    CARRT_INCLUDE_PROGDRIVE_IN_BUILD=1
    CARRT_INCLUDE_GOTODRIVE_IN_BUILD=1
    CARRT_NAVIGATE_USING_INERTIAL=1
    CARRT_NAVIGATE_USING_DEADRECKONING=0

    CARRT_VERSION_MAJOR=${VERSION_MAJOR} 
    CARRT_VERSION_MINOR=${VERSION_MINOR} 
    CARRT_VERSION_REVISION=${VERSION_REVISION} 
    CARRT_VERSION_STR="${CARRT_VERSION_STR}"

    CARRT_ENABLE_EVENTMANAGER_DEBUG=$<BOOL:${CARRT_ENABLE_EVENTMANAGER_DEBUG}>
    CARRT_ENABLE_GOTO_DEBUG=$<BOOL:${CARRT_ENABLE_GOTO_DEBUG}> 
    CARRT_ENABLE_NAVIGATION_MAP_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATION_MAP_DEBUG}>
    CARRT_ENABLE_NAVIGATOR_DEBUG=$<BOOL:${CARRT_ENABLE_NAVIGATOR_DEBUG}>
    CARRT_ENABLE_AVR_PATHFINDER_DEBUG=$<BOOL:${CARRT_ENABLE_AVR_PATHFINDER_DEBUG}>
)
add_dependencies( CarrtNoTest_IMU.elf  GitHeadInfo )
target_include_directories( CarrtNoTest_IMU.elf PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR} )
