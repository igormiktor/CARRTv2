/*
    Beep.cpp - Driver for CARRT's simple audio system

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



#include "Beep.h"


#include "AVRTools/GpioPinMacros.h"
#include "AVRTools/Pwm.h"
#include "AVRTools/SystemClock.h"

#include "CarrtPins.h"
#include "CarrtCallback.h"



void Beep::initBeep()
{
    setGpioPinModeOutput( pBeepPin );
    chirp();
}


void Beep::alert( unsigned int durationMs, unsigned int tone )
{
    writeGpioPinPwm( pBeepPin, tone );
    delayMilliseconds( durationMs );
    writeGpioPinPwm( pBeepPin, 0 );
}


void Beep::beep( unsigned int durationMs, unsigned int tone )
{
    writeGpioPinPwm( pBeepPin, tone );
    CarrtCallback::yieldMilliseconds( durationMs );
    writeGpioPinPwm( pBeepPin, 0 );
}


void Beep::chirp()
{
    writeGpioPinPwm( pBeepPin, kBeepDefaultChirpTone );
    delayMilliseconds( kBeepDefaultChirpDuration );
    writeGpioPinPwm( pBeepPin, 0 );
}


void Beep::errorChime()
{
    alert( 50 );
    delayMilliseconds( 50 );
    alert( 50 );
    delayMilliseconds( 50 );
    alert( 50 );
}


void Beep::triTone( unsigned int tone1, unsigned int tone2, unsigned int tone3 )
{
    writeGpioPinPwm( pBeepPin, tone1 );
    CarrtCallback::yieldMilliseconds( 50 );
    writeGpioPinPwm( pBeepPin, 0 );

    CarrtCallback::yieldMilliseconds( 75 );

    writeGpioPinPwm( pBeepPin, tone2 );
    CarrtCallback::yieldMilliseconds( 100 );
    writeGpioPinPwm( pBeepPin, 0 );

    CarrtCallback::yieldMilliseconds( 75 );

    writeGpioPinPwm( pBeepPin, tone3 );
    CarrtCallback::yieldMilliseconds( 150 );
    writeGpioPinPwm( pBeepPin, 0 );
}


void Beep::beepOn( unsigned int tone )
{
    writeGpioPinPwm( pBeepPin, tone );
}


void Beep::beepOff()
{
    writeGpioPinPwm( pBeepPin, 0 );
}
