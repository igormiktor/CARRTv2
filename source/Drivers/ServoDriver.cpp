/*
    ServoDriver.cpp - Functions for controlling the servo that the 
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





/***************************************************

    Modified by Igor Mikolic-Torreira
    
    Based on the Adafruit library for their 
    16-channel PWM & Servo driver

****************************************************/


/***************************************************
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "ServoDriver.h"

#include <math.h>

#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"




const uint8_t kServoDriverI2cAddress    = 0x40;


const uint8_t PCA9685_SUBADR1           = 0x02;
const uint8_t PCA9685_SUBADR2           = 0x03;
const uint8_t PCA9685_SUBADR3           = 0x04;

const uint8_t PCA9685_MODE1             = 0x0;
const uint8_t PCA9685_PRESCALE          = 0xFE;

const uint8_t LED0_ON_L                 =  0x06;
const uint8_t LED0_ON_H                 =  0x07;
const uint8_t LED0_OFF_L                =  0x08;
const uint8_t LED0_OFF_H                =  0x09;

const uint8_t ALLLED_ON_L               =  0xFA;
const uint8_t ALLLED_ON_H               =  0xFB;
const uint8_t ALLLED_OFF_L              =  0xFC;
const uint8_t ALLLED_OFF_H              =  0xFD;







namespace
{
    uint8_t read8( uint8_t addr );
    void write8( uint8_t addr, uint8_t d );


    uint8_t read8( uint8_t addr )
    {
        uint8_t temp;
        I2cMaster::readSync( kServoDriverI2cAddress, addr, 1, &temp );
        return temp;
    }

    void write8( uint8_t addr, uint8_t d )
    {
        I2cMaster::writeSync( kServoDriverI2cAddress, addr, d );
    }

};



void ServoDriver::init()
{
    reset();
}



void ServoDriver::reset()
{
    write8( PCA9685_MODE1, 0x0 );
}



void ServoDriver::setPWMFreq( float freq )
{
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    uint8_t prescale = floor( prescaleval + 0.5 );

    uint8_t oldmode = read8( PCA9685_MODE1 );
    uint8_t newmode = ( oldmode & 0x7F ) | 0x10;    // sleep
    write8( PCA9685_MODE1, newmode );               // go to sleep
    write8( PCA9685_PRESCALE, prescale );           // set the prescaler
    write8( PCA9685_MODE1, oldmode );
    delayMilliseconds( 5 );
    write8( PCA9685_MODE1, oldmode | 0xa1 );        //  This sets the MODE1 register to turn on auto increment.
}



void ServoDriver::setPWM( uint8_t num, uint16_t on, uint16_t off )
{
    uint8_t data[4];

    data[0] = on;
    data[1] = on >> 8;
    data[2] = off;
    data[3] = off >> 8;
    I2cMaster::writeSync( kServoDriverI2cAddress, ( LED0_ON_L + 4*num ), data, 4 );
}


