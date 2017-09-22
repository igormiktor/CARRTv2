/*
    TestMenuStates.cpp - Test Menu States for CARRT

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



#if CARRT_INCLUDE_TESTS_IN_BUILD



#include "TestMenuStates.h"

#include <avr/pgmspace.h>

#include "TestStates.h"
#include "WelcomeMenuStates.h"







namespace
{
    //                                      1234567890123456
    const PROGMEM char sTestMenuTitle[]  = "Select Test";
    const PROGMEM char sTestMenuItem00[] = "Exit...";
    const PROGMEM char sTestMenuItem01[] = "Memory";
    const PROGMEM char sTestMenuItem02[] = "1/4 Sec Events";
    const PROGMEM char sTestMenuItem03[] = "1 Sec Events";
    const PROGMEM char sTestMenuItem04[] = "8 Sec Events";
    const PROGMEM char sTestMenuItem05[] = "Beep";
    const PROGMEM char sTestMenuItem06[] = "Temp Sensor";
    const PROGMEM char sTestMenuItem07[] = "Batt LEDs";
    const PROGMEM char sTestMenuItem08[] = "Motor Batt";
    const PROGMEM char sTestMenuItem09[] = "CPU Batt";
    const PROGMEM char sTestMenuItem10[] = "Sonar";
    const PROGMEM char sTestMenuItem11[] = "Lidar";
    const PROGMEM char sTestMenuItem12[] = "Range Scan";
    const PROGMEM char sTestMenuItem13[] = "Compass";
    const PROGMEM char sTestMenuItem14[] = "Accelerometer";
    const PROGMEM char sTestMenuItem15[] = "Gyroscope";
    const PROGMEM char sTestMenuItem16[] = "Drive Fwd/Rev";
    const PROGMEM char sTestMenuItem17[] = "Drive Left/Right";
    const PROGMEM char sTestMenuItem18[] = "Error Handling";
    const PROGMEM char sTestMenuItem19[] = "Nav. Rotation";
    const PROGMEM char sTestMenuItem20[] = "Nav. Drive";


    const PROGMEM MenuList sTestMenu[] =
    {
        { sTestMenuItem01,  1 },
        { sTestMenuItem02,  2 },
        { sTestMenuItem03,  3 },
        { sTestMenuItem04,  4 },
        { sTestMenuItem05,  5 },
        { sTestMenuItem06,  6 },
        { sTestMenuItem07,  7 },
        { sTestMenuItem08,  8 },
        { sTestMenuItem09,  9 },
        { sTestMenuItem10,  10 },
        { sTestMenuItem11,  11 },
        { sTestMenuItem12,  12 },
        { sTestMenuItem13,  13 },
        { sTestMenuItem14,  14 },
        { sTestMenuItem15,  15 },
        { sTestMenuItem16,  16 },
        { sTestMenuItem17,  17 },
        { sTestMenuItem18,  18 },
        { sTestMenuItem19,  19 },
        { sTestMenuItem20,  20 },

        { sTestMenuItem00,  0 }
    };


    State* getTestState( uint8_t menuId )
    {
        switch ( menuId )
        {
            case 0:
                return new WelcomeState;

            case 1:
                return new AvailableMemoryTestState;

            case 2:
                return new Event1_4TestState;

            case 3:
                return new Event1TestState;

            case 4:
                return new Event8TestState;

            case 5:
                return new BeepTestState;

            case 6:
                return new TempSensorTestState;

            case 7:
                return new BatteryLedTestState;

            case 8:
                return new MotorBatteryVoltageTestState;

            case 9:
                return new CpuBatteryVoltageTestState;

            case 10:
                return new SonarTestState;

            case 11:
                return new LidarTestState;

            case 12:
                return new RangeScanTestState;

            case 13:
                return new CompassTestState;

            case 14:
                return new AccelerometerTestState;

            case 15:
                return new GyroscopeTestState;

            case 16:
                return new MotorFwdRevTestState;

            case 17:
                return new MotorLeftRightTestState;

            case 18:
                return new ErrorTestState;

            case 19:
                return new NavigatorRotateTestState;

            case 20:
                return new NavigatorDriveTestState;

            default:
                return 0;
        }
    }
}





TestMenuState::TestMenuState() :
MenuState( sTestMenuTitle, sTestMenu, sizeof( sTestMenu ) / sizeof( MenuItem ), getTestState )
{
    // Nothing else to do
}


#endif  // CARRT_INCLUDE_TESTS_IN_BUILD








