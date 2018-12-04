# CARRT Navigation Notes


## Overview

CARRT has two different navigation modes. The first mode is inertial navigation,
using the accelerometer, magnetometer and gyroscope as an inertial measurement unit,
estimating distance while moving by (double) integration of acceleration.  The
second mode is dead reckoning, which still uses the magnetometer and gyroscope for
orientation but estimates distances while moving by a fixed, empirically-derived
formula that converts time into distance.  The mode is chosen at compile time by
#defining either CARRT_NAVIGATE_USING_INERTIAL or CARRT_NAVIGATE_USING_DEADRECKONING.


## Common to both Navigation Modes

Underlying both of these methods are a series of navigation elements that remain fixed.  These are:

* Coordinate system: North corresponds to the positive X-axis; west corresponds to the positive
Y-axis.  This allows the 0 direction to map to North and enables the standard conversions between
Cartesian (x,y) coordinates and Polar (r,&theta;) coordinates.

* Angles: Computation uses standard mathematical angles (measured counter-clockwise from the X-axis)
in radians. Conversion to compass angles is straightforward:

    compass = (360 - &theta; * 180 / pi) modulo 360

* Units: The units for X- and Y-coordinates are internally maintained in meters.  Velocity is in
m/s, and acceleration is in m/s^2.

* Update rate: Navigation parameters are updated every 1/8 second.  The update reads the accelerometer,
magnetometer, and gyroscope and updates the orientation and position of CARRT.


Both navigation modes execute the following overall process:

1. Read the accelerometer, magnetometer, and gryoscope.

2. Estimate the current compass heading from the accelerometer and magnetometerUpdateRate data.
(A trick in this computation is that the cross product of the magnetometer and the accelerometer output vectors gives an East vector.)

3. Estimate the current compass heading by integrating the gyroscope's angular rate data.

4. Combine the two compass estimates to get a "best" estimate for the current compass heading

5. Use the new heading to convert from local, relative (ahead/downrange, to-the-left/crossrange)
coordinates produced by the accelerometer to absolute (N, W) coordinates used by the Navigator.

6. Using a velocity estimate (obtained differently for the different navigation modes), estimate
the distance traveled and update the (N, W) coordinates of CARRT's current position


The computations in the above steps can be confusing because the accelerometer, magnetometer, and
gryoscope outputs are in local, CARRT-centric, CARRT-oriented coordinates.   But the coordinates that
matter are absolue (N, W) coordinates.  The key to converting from local coordinates
to absolute coordinates is the fact that the cross product of the magnetometer and the accelerometer
output vectors gives an East vector in local coordinates.  The magnetormeter output is a north vector
in local coordinates and with those two vectors in hand, the local-to-global conversion is straightforward.

## Inertial Navigation Mode

Inertial navigation mode follows the following proces

## Dead-Reckoning Navigation Mode

