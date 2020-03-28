/*
    SonarTest.cpp - Testing harness for the Sonar driver.  This also
    serves to test the Servo driver.

    Copyright (c) 2020 Igor Mikolic-Torreira.  All right reserved.

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
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Sonar.h"





int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Serial0 out;
    out.start( 115200 );

    Sonar::init();

    delayMilliseconds( 1000 );

    out.println( "Sonar test..." );

    int semiSweep = 70;

    while ( 1 )
    {
        for ( int n = -semiSweep; n <= semiSweep; n += 10 )
        {
            Sonar::slew( n );
            delayMilliseconds( 1000 );
            int rng = Sonar::getDistanceInCm();
            out.print( "angle = " );
            out.print( n );
            out.print( ";  range = " );
            out.print( rng );
            out.println( " cm." );
        }

        delayMilliseconds( 1000 );
        Sonar::slew( 0 );
        delayMilliseconds( 5000 );
    }
}



