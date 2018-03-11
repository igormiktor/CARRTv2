/*
    DriveParam.h - Parameters related to CARRT's driving

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
    // Speeds are with full battery charge

    /*
     * Full-speed straight-drive approximation derived from following measurements
     *
     *  time (sec)          distance (cm)
     *      1                   23
     *      2                   78
     *      3                   118
     *      4                   159
     *      5                   198
     *
     *  Straightline fit is very good:  m = 39.90 cm/sec and b = -1.30 cm
     */


    float   kFullSpeedCmPerSec      = 39.90;        // cm/sec
    float   kFullSpeedCmPerQtrSec   = 39.90 / 4.0;  // cm/sec
    float   kFullSpeedIntercept     = -1.30;        // cm


};


#endif  // DriveParam
