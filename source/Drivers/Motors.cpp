/*
    Motors.cpp - Functions for controlling CARRT's drive motors

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




/*
    Inspired by, and had its origins in, the Adafruit library for the Adafruit
    Motor Shield v1, but the code has been heavily modified and specialized for CARRT.
    So much so that the code has practically no resemblance to the Adafruit original, but
    I still wanted to acknowledge the original...
*/




#include "Motors.h"

#include "CarrtPins.h"



// Extend namespace with additional functions and such
namespace Motors
{

    /*
    * The right-rear motor is particularly faster than the other moters,
    * so speed settings for it are adjusted
    */

    uint8_t kRightRearSpeedAdjust   = 12;



    // Bit positions in the 74HCT595 shift register output

    // Rear Right Motor
    const uint8_t kMotor1_A     = 2;
    const uint8_t kMotor1_B     = 3;

    // Front Right Motor
    const uint8_t kMotor2_A     = 1;
    const uint8_t kMotor2_B     = 4;

    // Front Left Motor
    const uint8_t kMotor3_A     = 5;
    const uint8_t kMotor3_B     = 7;

    // Rear Left Motor
    const uint8_t kMotor4_A     = 0;
    const uint8_t kMotor4_B     = 6;


    enum
    {
        kRightRearMotorNbr      = 1,
        kRightFrontMotorNbr     = 2,
        kLeftFrontMotorNbr      = 3,
        kLeftRearMotorNbr       = 4
    };


    uint8_t sLatchState;


    void enable74HCT595();

    void transmitLatch();

    void initMotorPWM();


#if CARRT_TEST_INDIVIDUAL_MOTORS

    void motorCommand( uint8_t motorA, uint8_t motorB, uint8_t cmd );

#endif

};



void Motors::transmitLatch()
{
    setGpioPinLow( pMotorLatchPin );

    setGpioPinLow( pMotorDataPin );

    for ( uint8_t i = 0; i < 8; i++ )
    {
        setGpioPinLow( pMotorClockPin );

        if ( sLatchState & ( 1 << (7-i) ) )
        {
            setGpioPinHigh( pMotorDataPin );
        }
        else
        {
            setGpioPinLow( pMotorDataPin );
        }

        setGpioPinHigh( pMotorClockPin );
    }

    setGpioPinHigh( pMotorLatchPin );
}



void Motors::enable74HCT595()
{
    // Set up the 74HCT595
    setGpioPinModeOutput( pMotorLatchPin );
    setGpioPinModeOutput( pMotorEnablePin );
    setGpioPinModeOutput( pMotorDataPin );
    setGpioPinModeOutput( pMotorClockPin );

    sLatchState = 0;

    // "Reset" the 74HCT595 H-bridge
    transmitLatch();

    // enable the chip outputs!
    setGpioPinLow( pMotorEnablePin );
}



void Motors::initMotorPWM()
{
    // Front Right Motor and Rear Left Motor
    // Use PWM on Arduino pin 5; on Arduino Mega, pin 5 is PE3 (OC3A)
    // Use PWM on Arduino pin 3; on Arduino Mega, pin 3 is PE5 (OC3C)
    // Phase correct PWM, prescale factor 8
    TCCR3A |= ( 1 << COM3A1 ) | ( 1 << COM3C1 ) | ( 1 << WGM30 );   // turn on OC3A and OC3C
    TCCR3B = ( 1 << CS31 );
    OCR3A = 0;
    OCR3C = 0;

    // Front Left Motor
    // Use PWM on Arduino pin 6; on arduino mega, pin 6 is PH3 (OC4A)
    // Phase correct PWM, prescale factor 8
    TCCR4A |= ( 1 << COM4A1 ) | ( 1 << WGM40 );                     // turn on OC4A
    TCCR4B = ( 1 << CS41 );
    OCR4A = 0;

    // Rear Right Motor
    // Use PWM on Arduino pin 11; on arduino mega, pin 11 is PB5 (OC1A)
    // Phase correct PWM, prescale factor 8
    TCCR1A |= ( 1 << COM1A1 ) | ( 1 << WGM10 );                     // turn on OC1A
    TCCR1B = ( 1 << CS11 );
    OCR1A = 0;

    // Set all pins to output mode
    setGpioPinModeOutput( pRearLeftMotorSpeedPin );                 // Pin 05
    setGpioPinModeOutput( pFrontRightMotorSpeedPin );               // Pin 03
    setGpioPinModeOutput( pFrontLeftMotorSpeedPin );                // Pin 06
    setGpioPinModeOutput( pRearRightMotorSpeedPin );                // Pin 11
}



void Motors::setSpeedAllMotors( uint8_t s )
{
    OCR1A = s - kRightRearSpeedAdjust;
    OCR3C = s;
    OCR4A = s;
    OCR3A = s;
}



void Motors::init()
{
    enable74HCT595();

    // Set all motor pins to 0
    sLatchState &= ~_BV( kMotor1_A ) & ~_BV( kMotor1_B )
                & ~_BV( kMotor2_A ) & ~_BV( kMotor2_B )
                & ~_BV( kMotor3_A ) & ~_BV( kMotor3_B )
                & ~_BV( kMotor4_A ) & ~_BV( kMotor4_B );

    transmitLatch();

    initMotorPWM();

    setSpeedAllMotors( Motors::kFullSpeed );
}



void Motors::goForward()
{
    sLatchState |=  _BV( kMotor1_A ) | _BV( kMotor2_A ) | _BV( kMotor3_A ) | _BV( kMotor4_A );
    sLatchState &= ~( _BV( kMotor1_B ) | _BV( kMotor2_B ) | _BV( kMotor3_B ) | _BV( kMotor4_B ) );

    transmitLatch();
}



void Motors::goBackward()
{
    sLatchState &= ~( _BV( kMotor1_A ) | _BV( kMotor2_A ) | _BV( kMotor3_A ) | _BV( kMotor4_A ) );
    sLatchState |=  _BV( kMotor1_B ) | _BV( kMotor2_B ) | _BV( kMotor3_B ) | _BV( kMotor4_B );

    transmitLatch();
}



void Motors::stop()
{
    // A and B both low
    sLatchState &= ~( _BV( kMotor1_A ) | _BV( kMotor1_B )
                | _BV( kMotor2_A ) | _BV( kMotor2_B )
                | _BV( kMotor3_A ) | _BV( kMotor3_B )
                | _BV( kMotor4_A ) | _BV( kMotor4_B ) );

    transmitLatch();
}



void Motors::rotateLeft()
{
    // Right side goes forward
    sLatchState |=  _BV( kMotor1_A ) | _BV( kMotor2_A );
    sLatchState &= ~( _BV( kMotor1_B ) | _BV( kMotor2_B ) );

    // Left side goes backward
    sLatchState &= ~( _BV( kMotor3_A ) | _BV( kMotor4_A ) );
    sLatchState |=  _BV( kMotor3_B ) | _BV( kMotor4_B );

    transmitLatch();
}



void Motors::rotateRight()
{
    // Right side goes backward
    sLatchState &= ~( _BV( kMotor1_A ) | _BV( kMotor2_A ) );
    sLatchState |=  _BV( kMotor1_B ) | _BV( kMotor2_B );

    // Left side goes forward
    sLatchState |=  _BV( kMotor3_A ) | _BV( kMotor4_A );
    sLatchState &= ~( _BV( kMotor3_B ) | _BV( kMotor4_B ) );

    transmitLatch();
}





#if CARRT_TEST_INDIVIDUAL_MOTORS


// cppcheck-suppress unusedFunction
void Motors::setRearRightMotorSpeed( uint8_t s )
{
    // Use PWM on Arduino pin 11; on arduino mega, pin 11 is PB5 (OC1A)
    OCR1A = s - kRightRearSpeedAdjust;
}


// cppcheck-suppress unusedFunction
void Motors::setFrontRightMotorSpeed( uint8_t s )
{
    // Use PWM on Arduino pin 3; on arduino mega, pin 3 is PE5 (OC3C)
    OCR3C = s;
}


// cppcheck-suppress unusedFunction
void Motors::setFrontLeftMotorSpeed( uint8_t s )
{
    // Use PWM on Arduino pin 6; on arduino mega, pin 6 is PH3 (OC4A)
    OCR4A = s;
}


// cppcheck-suppress unusedFunction
void Motors::setRearLeftMotorSpeed( uint8_t s )
{
    // Use PWM on Arduino pin 5; on arduino mega, pin 5 is PE3 (OC3A)
    OCR3A = s;
}


// cppcheck-suppress unusedFunction
void Motors::runRearRightMotor( uint8_t cmd )
{
    motorCommand( kMotor1_A, kMotor1_B, cmd );
}

// cppcheck-suppress unusedFunction
void Motors::runFrontRightMotor( uint8_t cmd )
{
    motorCommand( kMotor2_A, kMotor2_B, cmd );
}

// cppcheck-suppress unusedFunction
void Motors::runFrontLeftMotor( uint8_t cmd )
{
    motorCommand( kMotor3_A, kMotor3_B, cmd );
}

// cppcheck-suppress unusedFunction
void Motors::runRearLeftMotor( uint8_t cmd )
{
    motorCommand( kMotor4_A, kMotor4_B, cmd );
}


void Motors::motorCommand( uint8_t motorA, uint8_t motorB, uint8_t cmd )
{
    switch ( cmd )
    {
        case kCmdForward:
            sLatchState |=  _BV( motorA );
            sLatchState &= ~_BV( motorB );
            transmitLatch();
            break;

        case kCmdBackward:
            sLatchState &= ~_BV( motorA );
            sLatchState |=  _BV( motorB );
            transmitLatch();
            break;

        case kCmdBrake:
        case kCmdRelease:
            // A and B both low
            sLatchState &= ~_BV( motorA );
            sLatchState &= ~_BV( motorB );
            transmitLatch();
            break;
    }
}


#endif
