/*
    ServoDriver.h - Functions for controlling the servo that the 
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




#ifndef ServoDriver_H
#define ServoDriver_H

#include <stdint.h>




namespace ServoDriver
{
    
    // Servo pin assignments
    enum ServoPins
    {
        kRangeSensorServoPin = 0
    };
    

    void init();
    
    void reset();
    
    void setPWMFreq( float freq );
    
    void setPWM( uint8_t num, uint16_t on, uint16_t off );

};


#endif
