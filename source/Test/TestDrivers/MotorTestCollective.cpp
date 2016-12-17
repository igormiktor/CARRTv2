/*
    MotorTestCollective.cpp - Testing harness for the motor driver.

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




#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"

#include "Drivers/Motors.h"


int main()
{
    initSystem();
    initSystemClock();

    Motors::init();

	delayMilliseconds( 1000 );

    while ( 1 )
    {
        Motors::goForward();
        delayMilliseconds( 5000 );

        Motors::stop();
        delayMilliseconds( 1000 );

        Motors::goBackward();
        delayMilliseconds( 5000 );

        Motors::stop();
        delayMilliseconds( 1000 );

        Motors::rotateLeft();
        delayMilliseconds( 5000 );

        Motors::stop();
        delayMilliseconds( 1000 );

        Motors::rotateRight();
        delayMilliseconds( 5000 );

        Motors::stop();
        delayMilliseconds( 1000 );
    }
}


