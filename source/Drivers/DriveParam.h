/*
    DriveParam.h - Parameters and DR functions related to CARRT's driving

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





#ifndef DriveParam_h
#define DriveParam_h

#include <inttypes.h>


namespace DriveParam
{


    float distCmAtFullSpeedGivenTime( float seconds );

    float timeSecAtFullSpeedGivenDistanceCm( float cm );

    inline float timeSecAtFullSpeedGivenDistanceM( float meters )
    { return timeSecAtFullSpeedGivenDistanceCm( 100 * meters ); }

    float distMetersAtFullSpeedInEighthSec( int eighthsOfSec = 1 );

    inline float distMetersAtFullSpeedInQuarterSec()
    { return distMetersAtFullSpeedInEighthSec( 2 ); }

    float getFullSpeedMetersPerSec();


};


#endif  // DriveParam
