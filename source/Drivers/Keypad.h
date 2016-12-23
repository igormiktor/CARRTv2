/*
    Keypad.h - Driver for CARRT's Key Pad

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


#ifndef Keypad_h
#define Keypad_h


#include <stdint.h>



namespace Keypad
{
    enum Keys
    {
        kButton_Select          = 0x01,
        kButton_Right           = 0x02,
        kButton_Down            = 0x04,
        kButton_Up              = 0x08,
        kButton_Left            = 0x10,

        // Chords (finger feasible ones; often require holding down the buttons)
        kChord_Left_Up          = kButton_Left | kButton_Up,
        kChord_Down_Right       = kButton_Down | kButton_Right,
        kChord_Left_Right       = kButton_Left | kButton_Right,
        kChord_Right_Select     = kButton_Right | kButton_Select,
        kChord_Up_Down          = kButton_Up | kButton_Down,

        // Aliases
        kChord_Reset            = kChord_Right_Select,
        kChord_Pause            = kChord_Left_Right,
        kChord_Continue         = kChord_Up_Down,
        kChord_A                = kChord_Left_Up,
        kChord_B                = kChord_Down_Right
    };


    uint8_t readButtons();
};




#endif
