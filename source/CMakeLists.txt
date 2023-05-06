# CMakeLists.txt - cmake build file for CARRT

# Copyright (c) 2022 Igor Mikolic-Torreira.  All right reserved.

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


cmake_minimum_required( VERSION 3.5 )

set( AVR_UPLOADTOOL avrdude )
set( AVR_PROGRAMMER wiring )
set( AVR_UPLOADTOOL_PORT /dev/CARRT )
#set( AVR_UPLOADTOOL_PORT /dev/ArduinoMega01 )

set( AVR_MCU atmega2560 )
set( AVR_MCU_SPEED 16000000UL )

set( CMAKE_TOOLCHAIN_FILE $ENV{HOME}/.cmake/avr-gcc-toolchain.cmake )


project( CARRTv2 )

set( CMAKE_BUILD_TYPE Release )

set( VERSION_MAJOR 2 )
set( VERSION_MINOR 3 )
set( VERSION_REVISION 3 )


set( BUILD_DEBUG_VERSIONS ON )

message( "** Building with debugging is " ${BUILD_DEBUG_VERSIONS} )

# Various debugging options
#add_definitions( -DCARRT_ENABLE_EVENTMANAGER_DEBUG=1 )
add_definitions( -DCARRT_ENABLE_GOTO_DEBUG=1 )
#add_definitions( -DCARRT_ENABLE_NAVIGATION_MAP_DEBUG=1 )
#add_definitions( -DCARRT_ENABLE_NAVIGATOR_DEBUG=1 )
#add_definitions( -DCARRT_ENABLE_AVR_PATHFINDER_DEBUG=1 )



enable_language( ASM )

include_directories( "." "${CMAKE_CURRENT_BINARY_DIR}" )


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
        PauseState.cpp
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
        AVRTools/Writer.cpp
        AVRTools/Reader.cpp
        AVRTools/abi.cpp
        AVRTools/new.cpp
        AVRTools/I2cMaster.cpp
        AVRTools/I2cLcd.cpp
        AVRTools/SimpleDelays.S
        AVRTools/SPI.cpp
    )


set( ArvUtilsDbgSrcs
       AVRTools/USART0.cpp
    )


if( BUILD_DEBUG_VERSIONS )
    add_definitions( -DCARRT_ENABLE_DEBUG_SERIAL=1 )
    set( AvrUtilsSrcs ${AvrUtilsSrcs} AVRTools/USART0.cpp )
endif()


add_definitions( -DCARRT_INCLUDE_SPECIAL_MSG=0 )
add_definitions( -DCARRT_SPECIAL_MSG_TEXT="T. Tester" )


set( CARRT_VERSION_STR "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}" )

add_definitions( -DCARRT_VERSION_MAJOR=${VERSION_MAJOR} )
add_definitions( -DCARRT_VERSION_MINOR=${VERSION_MINOR} )
add_definitions( -DCARRT_VERSION_REVISION=${VERSION_REVISION} )
add_definitions( -DCARRT_VERSION_STR="${CARRT_VERSION_STR}" )

set( BuildInfo_h_fake "${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.h_fake" )
set( Git_Head_Hash_h  "${CMAKE_CURRENT_BINARY_DIR}/GitHeadHash.h" )

if( EXISTS ${BuildInfo_h_fake} )
    message( FATAL_ERROR "File \"${BuildInfo_h_fake}\" found, this should never be created, remove!" )
endif()

# a custom target that is always built
add_custom_target(
    BuildInfo
    DEPENDS ${BuildInfo_h_fake}
)

# Touches BuildInfo.h to ensure dependencie are built
add_custom_command(
    OUTPUT
        ${BuildInfo_h_fake}  # ensure we always run
        ${Git_Head_Hash_h}
    COMMAND
        echo  "// This is the git HEAD sha1 hash" > ${Git_Head_Hash_h}
    COMMAND
        echo \\" #define _CARRT_GIT_HEAD_SHA1 \\" \\\" `git rev-parse --short HEAD` \\\" >> ${Git_Head_Hash_h}
#   VERBATIM
)

# buildinfo.h is a generated file
set_source_files_properties(
    ${GitHead_Hash_h}
    PROPERTIES GENERATED TRUE
    HEADER_FILE_ONLY TRUE
)



add_avr_executable( Carrt ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( Carrt.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_TESTS_IN_BUILD=1 -DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_DEADRECKONING=1" )
add_dependencies( Carrt.elf  BuildInfo )

add_avr_executable( CarrtNoTest ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( CarrtNoTest.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_DEADRECKONING=1" )
add_dependencies( CarrtNoTest.elf  BuildInfo )


add_avr_executable( Carrt_IMU ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( Carrt_IMU.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_TESTS_IN_BUILD=1 -DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_INERTIAL=1" )
add_dependencies( Carrt_IMU.elf  BuildInfo )

add_avr_executable( CarrtNoTest_IMU ${CarrtSrcs} ${DriverSrcs} ${UtilSrcs} ${AvrUtilsSrcs} )
set_target_properties( CarrtNoTest_IMU.elf PROPERTIES COMPILE_FLAGS
    "-DCARRT_INCLUDE_PROGDRIVE_IN_BUILD=1 -DCARRT_INCLUDE_GOTODRIVE_IN_BUILD=1 -DCARRT_NAVIGATE_USING_INERTIAL=1" )
add_dependencies( CarrtNoTest_IMU.elf  BuildInfo )
