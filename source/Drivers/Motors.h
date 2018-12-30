/*
    Motors.h - Functions for controlling CARRT's drive motors

    Copyright (c) 2018 Igor Mikolic-Torreira.  All right reserved.

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





#ifndef Motors_h
#define Motors_h

#include <inttypes.h>


namespace Motors
{

    const uint8_t kFullSpeed = 255;
    const uint8_t kHalfSpeed = 200;

    void init();

    void setSpeedAllMotors( uint8_t s );

    void goForward();
    void goBackward();
    void stop();

    void rotateLeft();
    void rotateRight();


#if CARRT_TEST_INDIVIDUAL_MOTORS

    enum
    {
        kCmdForward = 1,
        kCmdBackward,
        kCmdBrake,
        kCmdRelease
    };

    void setRearRightMotorSpeed( uint8_t s );
    void setFrontRightMotorSpeed( uint8_t s );
    void setFrontLeftMotorSpeed( uint8_t s );
    void setRearLeftMotorSpeed( uint8_t s );

    void runRearRightMotor( uint8_t cmd );
    void runRearLeftMotor( uint8_t cmd );
    void runFrontRightMotor( uint8_t cmd );
    void runFrontLeftMotor( uint8_t cmd );

#endif

};


#endif
