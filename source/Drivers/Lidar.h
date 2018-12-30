/*
    Lidar.h - Functions for controlling CARRT's Garmin Lidar Lite v3 range sensor

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





#ifndef Lidar_h
#define Lidar_h

#include <stdint.h>
#include <limits.h>





namespace Lidar
{
    // getDistanceInCm() returns this if no valid range
    const int kNoValidDistance  = INT_MAX;


    const int kDefaultNbrMedianSamples = 5;


    // Modes of Lidar ranging operation
    enum Configuration
    {
        kDefault,
        kShortRangeAndHighSpeed,
        kShortRangeAndHighestSpeed,
        kDefaultRangeAndHigherSpeedAtShortRange,
        kMaximumRange,
        kHighSensitivityButHigherError,
        kLowSensitivityButLowerError
    };



    void init();


    // Lidar takes approximately 22ms to reset
    // Does not change slew angle (only Lidar internal state)
    int reset();


    // Set the Lidar configuration
    int setConfiguration( Configuration config );


    // Slew angles measured relative to 000 = straight ahead;
    // left/counter-clockwise = negative, right/clockwise = positive
    int slew( int angleDegrees );

    // Return the current slew angle
    int getCurrentAngle();


    // Get the range in cm from a single range ping
    // Negative value means no valid range obtained
    int getDistanceInCm( int* distInCm, bool useBiasCorrection = true );


    // Get the range in cm from the median of a set of pings
    // Negative value means no valid range obtained
    int getMedianDistanceInCm( int* distInCm, uint8_t nbrMedianSamples = kDefaultNbrMedianSamples, bool useBiasCorrection = true );


};



#endif
