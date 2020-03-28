/*
    Sonar.h - Functions for controlling CARRT's servo-mounted ultrasonic range sensor

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





#ifndef Sonar_h
#define Sonar_h

#include <stdint.h>
#include <limits.h>





namespace Sonar
{

    const int       kNoSonarEcho                = INT_MAX;
    const uint8_t   kNbrMedianSamples           = 3;

    void init();

    // Slew angles measured relative to 000 = straight ahead;
    // left/counter-clockwise = positive, right/clockwise = negative
    int slew( int angleDegrees );

    int getCurrentAngle();

    // Get the distance, specifying how many samples to take
    int getDistanceInCm( uint8_t nbrSamples );

    // Takes ~75ms to execute for a target at 2m
    inline int getDistanceInCm()               { return getDistanceInCm( kNbrMedianSamples ); }

    // Takes ~ 5ms to execute for a target at 2m
    int getSinglePingDistanceInCm();

};



#endif
