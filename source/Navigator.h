/*
    Navigator.h - An Inertial Navigation module for CARRT

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





#ifndef Navigator_h
#define Navigator_h


#include "Utils/VectorUtils.h"


/*
 *
 * COORDINATE SYSTEM FOR CARRT NAVIGATION
 *
 * X coord = North > 0; South < 0
 *
 * Y coord = West > 0; East < 0
 *
 * This allows the 0 direction to map to North
 * and enable the standard (x,y)->(r,theta) equivalence.
 *
 * Also, standard mathematical angles translate to
 * compass angles through a simple transformation:
 *
 * compass = (360 - theta * 180 / pi) modulo 360
 *
 * Current heading is maintained as a normal
 * compass heading, 0 = North and 0 <= heading < 360
 *
 * Units for X- and Y-coordinates are internally in meters.
 *
 * Velocity is in m/s and acceleration in m/s^2
 *
 */




namespace Navigator
{

    void init();

    void doNavUpdate();

    void doDriftCorrection();

    float getCurrentHeading();

    int convertToCompassAngle( float mathAngle );

    // Forward and left are positive; backward and right are negative
    Vector2Float convertRelativeToAbsoluteCoordsCm( int downRange, int crossRange );
    Vector2Float convertRelativeToAbsoluteCoordsMeter( int downRange, int crossRange );

    Vector2Float getCurrentPosition();
    Vector2Float getCurrentPositionCm();

    Vector2Float getCurrentVelocity();

    Vector2Float getCurrentAcceleration();

    Vector3Int getRestStateAcceleration();

    Vector3Int getRestStateAngularRate();

    void movingStraight();

    void movingTurning();

    bool isMoving();

    void stopped();

    void reset();

    void hardReset();

};



#endif
