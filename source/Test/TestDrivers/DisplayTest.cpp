/*
    DisplayTest.cpp - Testing harness for the LCD display driver.

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




#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"

#include "Drivers/Display.h"


int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Display::init();

    delayMilliseconds( 1000 );

    while ( 1 )
    {
        Display::displayTopRow( "ABCDEFGHIJKLMNOP" );
        Display::displayBottomRow( "abcdefghijklmnop" );

        delayMilliseconds( 5000 );

        Display::clearTopRow();
        delayMilliseconds( 1000 );
        Display::clearBottomRow();
        delayMilliseconds( 3000 );

        Display::displayTopRow( "01234567890ABCDE" );
        Display::setCursor( 1, 0 );
        Display::print( "abcdefghijklmnop" );

        delayMilliseconds( 5000 );

        Display::clearTopRow();
        Display::setCursor( 0, 8 );
        Display::print( 'Z' );
        Display::displayBottomRow( "01234567890ABCDE" );

        delayMilliseconds( 5000 );

        Display::clear();
        Display::displayTopRow( "255 base 10" );
        Display::setCursor( 1, 0 );
        Display::print( 255 );

        delayMilliseconds( 5000 );

        Display::clear();
        Display::displayTopRow( "255 base 2" );
        Display::setCursor( 1, 0 );
        Display::print( 255, Display::kBin );

        delayMilliseconds( 5000 );

        Display::clear();
        Display::displayTopRow( "255 base 8" );
        Display::setCursor( 1, 0 );
        Display::print( 255, Display::kOct );

        delayMilliseconds( 5000 );

        Display::clear();
        Display::displayTopRow( "255 base 16" );
        Display::setCursor( 1, 0 );
        Display::print( 255, Display::kHex );
        delayMilliseconds( 5000 );

        Display::clear();
        Display::displayTopRow( "-123456789L" );
        Display::setCursor( 1, 0 );
        Display::print( -123456789L );

        delayMilliseconds( 5000 );

        Display::clear();
        Display::displayTopRow( "123456789UL" );
        Display::setCursor( 1, 0 );
        Display::print( 123456789UL );

        delayMilliseconds( 5000 );

        Display::clear();
        Display::displayTopRow( "2.7182818289" );
        Display::setCursor( 1, 0 );
        Display::print( 2.7182818289 );

        delayMilliseconds( 5000 );
    }
}



