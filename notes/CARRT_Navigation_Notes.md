# CARRT Navigation Notes


## Overview

CARRT has two different navigation modes. The first mode is inertial navigation,
using the accelerometer, compass and gyroscope as an inertial measurement unit,
estimating distance while moving by (double) integration of acceleration.  The
second mode is dead reckoning, which still uses the compass and gyroscope for
orientation but estimates distances while moving by a fixed, empirically-derived
formula that converts time into distance.  The mode is chosen at compile time by
#defining either CARRT_NAVIGATE_USING_INERTIAL or
CARRT_NAVIGATE_USING_DEADRECKONING.

Underlying both of these methods are a series of navigation elements that remain fixed.  These are:

* Coordinate system: North corresponds to the positive x-axis; west corresponds to the positive
y-axis.  This allows the 0 direction to map to North and enables the standard conversions between
Cartesian (x,y) coordiantes and Polar (r,&theta;) coordinates.

* Angles: Computation uses standard mathematical angles (measured counter-clockwise from the x-axis)
in radians. Conversion to compass angles is straightforward:

    compass = (360 - &theta; * 180 / pi) modulo 360

* Units: The units for X- and Y-coordinates are internally maintained in meters.  Velocity is in
m/s, and acceleration is in m/s^2.

* Update rate: Navigation parameters are updated every 1/8 second.  The update reads the accelerometer, compass,
and gyroscope and updates the orientation and position of CARRT.


