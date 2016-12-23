/*
    KeypadTest.cpp - Testing harness for the keypad driver.

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
#include "AVRTools/I2cMaster.h"

#include "Drivers/Keypad.h"
#include "Drivers/Display.h"


int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    Display::init();

    delayMilliseconds( 1000 );

    const int kMinTimeBetweenButtonChecks = 250;        // milliseconds
    unsigned long sNextTimeButtonClickAccepted = 0;

    Display::displayTopRow( "Buttom hit:" );

    while ( 1 )
    {

        uint8_t buttonHit = Keypad::readButtons();

        if ( buttonHit && millis() > sNextTimeButtonClickAccepted )
        {
            // Accept the button click

            // Rollover happens in about 50 days, so don't worry about it
            sNextTimeButtonClickAccepted = millis() + kMinTimeBetweenButtonChecks;

            Display::clearBottomRow();

            switch ( buttonHit )
            {
                case Keypad::kButton_Select:
                    Display::displayBottomRow( "Select" );
                    break;

                case Keypad::kButton_Right:
                    Display::displayBottomRow( "Right" );
                    break;

                case Keypad::kButton_Down:
                    Display::displayBottomRow( "Down" );
                    break;

                case Keypad::kButton_Up:
                    Display::displayBottomRow( "Up" );
                    break;

                case Keypad::kButton_Left:
                    Display::displayBottomRow( "Left" );
                    break;

                case Keypad::kChord_Reset:
                    Display::displayBottomRow( "Reset (chord)" );
                    break;

               case Keypad::kChord_Pause:
                    Display::displayBottomRow( "Pause (chord)" );
                    break;

                case Keypad::kChord_Continue:
                    Display::displayBottomRow( "Continue (chord)" );
                    break;

                case Keypad::kChord_A:
                    Display::displayBottomRow( "A (chord)" );
                    break;

                case Keypad::kChord_B:
                    Display::displayBottomRow( "B (chord)" );
                    break;

                 default:
                    Display::displayBottomRow( "???" );
                    break;
            }
        }

    }
}



