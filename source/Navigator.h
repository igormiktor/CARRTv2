/*
    Navigator.h - An Inertial Navigation module for CARRT

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





#ifndef Navigator_h
#define Navigator_h


#include "Utils/VectorUtils.h"



namespace Navigator
{

    void init();

    void doNavUpdate();

    void doDriftCorrection();

    float getCurrentHeading();

    Vector2Float getCurrentPosition();

    Vector2Float getCurrentSpeed();

    Vector2Float getCurrentAcceleration();

    Vector3Int getRestStateAcceleration();

    Vector3Int getRestStateAngularRate();

    void moving();

    void movingStraight();

    void movingTurning();

    bool isMoving();

    void stopped();

    void reset();

    void hardReset();

};



#endif
