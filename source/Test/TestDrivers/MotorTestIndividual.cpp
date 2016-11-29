/*
    MotorTestIndividual.cpp - Testing harness for the motor driver.

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



#if !(CARRT_TEST_MOTORS)
#error "CARRT_TEST_MOTORS must be #defined to 1 for the Individual Motor Test"
#endif

int main()
{
    initSystem();
    initSystemClock();

    Motors::init();

    while ( 1 )
    {
		delayMilliseconds( 1000 );

		Motors::runRearRightMotor( Motors::kCmdForward );
		delayMilliseconds( 1000 );

		Motors::runRearLeftMotor( Motors::kCmdForward );
		delayMilliseconds( 1000 );

		Motors::runFrontRightMotor( Motors::kCmdForward );
		delayMilliseconds( 1000 );

		Motors::runFrontLeftMotor( Motors::kCmdForward );
		delayMilliseconds( 1000 );

		Motors::runRearRightMotor( Motors::kCmdRelease );
		Motors::runRearLeftMotor( Motors::kCmdRelease );
		Motors::runFrontRightMotor( Motors::kCmdRelease );
		Motors::runFrontLeftMotor( Motors::kCmdRelease );
		delayMilliseconds( 1000 );


		Motors::runRearRightMotor( Motors::kCmdBackward );
		delayMilliseconds( 1000 );

		Motors::runRearLeftMotor( Motors::kCmdBackward );
		delayMilliseconds( 1000 );

		Motors::runFrontRightMotor( Motors::kCmdBackward );
		delayMilliseconds( 1000 );

		Motors::runFrontLeftMotor( Motors::kCmdBackward );
		delayMilliseconds( 1000 );

		Motors::runRearRightMotor( Motors::kCmdRelease );
		Motors::runRearLeftMotor( Motors::kCmdRelease );
		Motors::runFrontRightMotor( Motors::kCmdRelease );
		Motors::runFrontLeftMotor( Motors::kCmdRelease );
    }
}
