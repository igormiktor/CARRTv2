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
        kButton_Select       = 0x01,
        kButton_Right        = 0x02,
        kButton_Down         = 0x04,
        kButton_Up           = 0x08,
        kButton_Left         = 0x10
    };


    uint8_t readButtons();
};




#endif
