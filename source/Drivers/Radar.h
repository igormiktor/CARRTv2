/*
    Radar.h - Functions for controlling CARRT's servo-mounted ultrasonic range sensor

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





#ifndef Radar_h
#define Radar_h

#include <stdint.h>
#include <limits.h>





namespace Radar
{

    const int kNoRadarEcho = INT_MAX;

    void init();

    // Slew angles measured relative to 000 = straight ahead;
    // left/counter-clockwise = positive, right/clockwise = negative
    int slew( int angleDegrees );

    int getCurrentAngle();

    // Takes ~150ms to execute
    int getDistanceInCm();

};



#endif
