/*
    CarrtCallback.h - Various call-backs into the main CARRT routines

    Copyright (c) 2022 Igor Mikolic-Torreira.  All right reserved.

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




#ifndef CARRTCallback_h
#define CARRTCallback_h


#include <stdint.h>


namespace CarrtCallback
{


    /*
        Wait for a set amount of time while allowing CARRT to process events
        and do other housekeeping.
    */

    void yieldMilliseconds( uint16_t milliseconds = 100 );



    /*
        Is CARRT in motion?.
    */

    bool isMoving();


};


#endif
