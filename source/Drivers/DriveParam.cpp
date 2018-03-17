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
     *      1                    23
     *      2                    78
     *      3                   118
     *      4                   159
     *      5                   198
     *
     *  Straightline fit is very good for T >= 2 sec:
     *      m = 40.10 cm/sec and b = -2.10 cm
     *
     *  For t < 2 sec use quadratic form:
     *      d = at^2 + bt
     *  with a = 16.0 cm/s/s and b = 7.0 cm/s
     *  (derived from empirical fit to data for t=1 and t=2)
     *
     *  For t < 0.5, just zero
     */


    float   kFullSpeedCmPerSec      = 40.1;        // cm/sec
//    float   kFullSpeedCmPerQtrSec   = 40.1 / 4.0;  // cm/sec
    float   kFullSpeedIntercept     = -2.1;        // cm

    float   kFullSpeedAccel         = 16.0;         // cm/sec/sec
    float   kFullSpeedAccelV0       = 7.0;          // cm/sec
};





float DriveParam::distCmAtFullSpeedGivenTime( float seconds )
{
    if ( seconds < 0.5 )
    {
        return 0;
    }
    else if ( seconds < 2 )
    {
        return ( kFullSpeedAccel * seconds + kFullSpeedAccelV0 ) * seconds;
    }
    else
    {
        return kFullSpeedCmPerSec * seconds + kFullSpeedIntercept;
    }
}




float DriveParam::timeSecAtFullSpeedGivenDistance( float cm )
{
    float t = ( cm - kFullSpeedIntercept ) / kFullSpeedCmPerSec;

    if ( t < 2 )
    {
        // Need to recompute via quadratic formula (pick positive root )
        float tmp = kFullSpeedAccelV0 * kFullSpeedAccelV0 + 4 * kFullSpeedAccel * cm;
        tmp = -kFullSpeedAccelV0 + sqrt( tmp );
        t = tmp / ( 2 * kFullSpeedAccel );
    }

    if ( t < 0.5 )
    {
        t = 0;
    }

    return t;
}




