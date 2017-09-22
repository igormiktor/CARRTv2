/*
    Servo.cpp - Functions for controlling the servo that the
    range sensor is mounted on.

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




/*
    Inspired by, and had its origins in, the Adafruit library for the LSM303DLHC,
    but it has been heavily modified.
*/



#include "Servo.h"

#include <math.h>

#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"







// Extend the Servo namespace with private functions

namespace Servo
{

    const uint8_t kServoI2cAddress      = 0x40;


    const uint8_t kPCA9685_SubAdr1      = 0x02;
    const uint8_t kPCA9685_SubAdr2      = 0x03;
    const uint8_t kPCA9685_SubAdr3      = 0x04;

    const uint8_t kPCA9685_Mode1        = 0x0;
    const uint8_t kPCA9685_Prescale     = 0xFE;

    const uint8_t kLed_On_L             =  0x06;
    const uint8_t kLed_On_H             =  0x07;
    const uint8_t kLed_Off_L            =  0x08;
    const uint8_t kLed_Off_H            =  0x09;

    const uint8_t kAllLeds_On_L         =  0xFA;
    const uint8_t kAllLeds_On_H         =  0xFB;
    const uint8_t kAllLeds_Off_L        =  0xFC;
    const uint8_t kAllLeds_Off_H        =  0xFD;


    // Servo pin assignments
    const uint8_t  kRangeSensorServoPin = 0;


    uint8_t readByte( uint8_t addr );
    void writeByte( uint8_t addr, uint8_t d );



    uint8_t readByte( uint8_t addr )
    {
        uint8_t temp;
        I2cMaster::readSync( kServoI2cAddress, addr, 1, &temp );
        return temp;
    }


    void writeByte( uint8_t addr, uint8_t d )
    {
        I2cMaster::writeSync( kServoI2cAddress, addr, d );
    }


    int8_t mCurrentAngle;

    uint16_t convertToPulseLenFromDegreesRelative( int8_t degrees );

};



void Servo::init()
{
    reset();
}



void Servo::reset()
{
    writeByte( kPCA9685_Mode1, 0x0 );
}



void Servo::setPWMFreq( float freq )
{
    // Calculate the appropriate prescaler
    float prescaleValue = 25000000;
    prescaleValue /= 4096;
    prescaleValue /= freq;
    prescaleValue -= 1;
    uint8_t prescaler = floor( prescaleValue + 0.5 );

    uint8_t originalMode = readByte( kPCA9685_Mode1 );
    uint8_t sleepMode = ( originalMode & 0x7F ) | 0x10;

    // Set the PCA9685 to sleep so we can set the prescaler
    writeByte( kPCA9685_Mode1, sleepMode );
    writeByte( kPCA9685_Prescale, prescaler );

    // Restore the original mode
    writeByte( kPCA9685_Mode1, originalMode );
    delayMilliseconds( 5 );

    // Set the MODE1 registor to enable auto-increment
    writeByte( kPCA9685_Mode1, originalMode | 0xa1 );
}



void Servo::setPWM( uint16_t on, uint16_t off )
{
    uint8_t data[4];

    data[0] = on;
    data[1] = on >> 8;
    data[2] = off;
    data[3] = off >> 8;

    I2cMaster::writeSync( kServoI2cAddress, ( kLed_On_L + 4*kRangeSensorServoPin ), data, 4 );
}









// cppcheck-suppress unusedFunction
int Servo::getCurrentAngle()
{
    return mCurrentAngle;
}




int Servo::slew( int angleDegrees )
{
    // Protect against over slewing of the radar
    if ( angleDegrees > 85 )
    {
        angleDegrees = 85;
    }
    if ( angleDegrees < -85 )
    {
        angleDegrees = -85;
    }

    mCurrentAngle = angleDegrees;

    uint16_t pulseLen = convertToPulseLenFromDegreesRelative( mCurrentAngle );
    setPWM( 0, pulseLen );

    return mCurrentAngle;
}




uint16_t Servo::convertToPulseLenFromDegreesRelative( int8_t degrees )
{
/*
 *    -90 = 155
 *      0 = 381
 *     90 = 605
 *
 *     Later adjustment makes the center -3 deg off; translates to the -6 adjustment below
 */

    int16_t tmp = 5 * static_cast<int16_t>( degrees );
    tmp /= 2;
    tmp += 381-4;
    return static_cast<uint16_t>( tmp );
}





