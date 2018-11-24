/*
    DriveParam.cpp - Parameters and DR functions related to CARRT's driving

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





#include "DriveParam.h"

#include <math.h>



namespace DriveParam
{
    // Speeds are with full battery charge

    /*
     * Full-speed straight-drive approximation derived from following measurements
     *
     *  time (sec)          distance (cm)
     *      1                    36
     *      2                    71
     *      3                   108
     *      4                   144
     *      5                   179
     *
     *  Straightline fit is very good (r = 0.9999573277):
     *      m = 35.90 cm/sec and b = -0.10 cm
     */


    const float   kFullSpeedCmPerSec      = 35.90;        // cm/sec
    const float   kFullSpeedIntercept     = -0.10;        // cm
    const float     kMinTime                = 0.1;      // sec
};





float DriveParam::distCmAtFullSpeedGivenTime( float seconds )
{
    return ( seconds < kTimeTime ) ? 0.0 : kFullSpeedCmPerSec * seconds + kFullSpeedIntercept;
}




float DriveParam::timeSecAtFullSpeedGivenDistance( float cm )
{
    return ( cm - kFullSpeedIntercept ) / kFullSpeedCmPerSec;
}




float DriveParam::distMetersAtFullSpeedInQuarterSec()
{
    return (kFullSpeedCmPerSec / 4 + kFullSpeedIntercept) / 100;
}




float DriveParam::getFullSpeedMetersPerSec()
{
    return kFullSpeedCmPerSec / 100;
}