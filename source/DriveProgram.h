/*
    DriveProgram.h - A program for a CARRT drive represented as a singly-linked list

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



#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD


#ifndef DriveProgram_h
#define DriveProgram_h


#include <stdint.h>

class BaseProgDriveState;






namespace DriveProgram
{
    void init();

    void create();

    void purge();

    uint8_t len();

    bool isEmpty();

    void addAction( BaseProgDriveState* action );

    BaseProgDriveState* getProgramStart();

};



#endif


#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD
