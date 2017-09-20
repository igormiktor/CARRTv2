/*
    Servo.h - Functions for controlling the servo that the
    range sensor is mounted on.

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




#ifndef Servo_H
#define Servo_H

#include <stdint.h>




namespace Servo
{


    void init();

    void reset();

    void setPWMFreq( float freq );

    void setPWM( uint16_t on, uint16_t off );


    // Slew angles measured relative to 000 = straight ahead;
    // left/counter-clockwise = positive, right/clockwise = negative
    int slew( int angleDegrees );

    int getCurrentAngle();

};


#endif
