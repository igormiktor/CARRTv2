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

* Coordinate system: The global coordinate system used by CARRT for navigation places North along the
positive X-axis and West along the positive Y-axis.  This allows the 0 direction to map to North and
enables the standard conversions between Cartesian (x,y) coordinates and Polar (r,&theta;) coordinates.
All navigation is done in the global (N,W) coordinate system.

* Local coordinates: The accelerometer and magnetometer all provide data in local,
CARRT-centric 3-D coordinates in which the X-axis points in CARRT's forward direction, the Y-axis points
to CARRT's left, and the Z-axis points upward.  The gyroscope provides angular rates along these same
three local axes. The conversion from local coordinates to global coordinates is dynamic, changing
over time as CARRT's orientation in 3-space changes (although the primarily consideration is
orientation in the horizontal plane because CARRT usually doesn't drive on slopes--but the conversion
covers this case, too).

* Angles: Navigation internally uses standard mathematical angles (measured counter-clockwise from the
North, or X-axis) in radians. Conversion to compass angles is straightforward:

    compass = (360 - &theta; * 180 / pi) modulo 360

* Units: The units for global X- and Y-coordinates are internally maintained in meters.  Velocity is in
m/s, and acceleration is in m/s^2.

* Update rate: Navigation parameters are updated every 1/8 second.  The update reads the accelerometer,
magnetometer, and gyroscope and updates the orientation and position of CARRT.


Both navigation modes execute the following overall process:

1. Read the accelerometer, magnetometer, and gyroscope.

2. Estimate the current compass heading from the accelerometer and magnetometer data.  This step
also establishes the coordinate transformation between the local to global coordinate system.

3. Estimate the current compass heading by integrating the gyroscope's angular rate data.

4. Combine the two compass estimates to get a "best" estimate for the current compass heading.

5. Using a velocity estimate (obtained differently for the different navigation modes), estimate
the distance traveled and update the global (N,W) coordinates of CARRT's current position.


The coordinate transformation figured out in step 2 is essential to all other computations because all
measurement data is in 3-D local coordinates but everything has to be ultimately converted or computed
in the 2-D global (N,W) coordinate system. It isn't immediately obvious how to obvious how to get to
the global coordinate system from the measured accelerometer, magnetometer, and gryoscope vectors in local
coordinates.  The key to converting from local to global coordinates is the observation that the cross
product of the magnetometer and the accelerometer output vectors gives an East vector in local
coordinates.  This is true even if CARRT is on a slope.  The magnetometer output is a north vector in
local coordinates and these two vectors provide a basis in local coordinates for the global coordinate
system.  With this basis set in hand, the local-to-global conversion is straightforward.


## Inertial Navigation Mode

Inertial navigation mode implements the above process.  The first 4 steps are executed as described above.
To implement step 5, the acceleration from the accelerometer is converted from local to global (N,W)
coordinates and then integrated once and added the velocity from the prior time-step to obtain an
estimate of the current velocity vector.  The current velocity vector is integrated a second time and
added the position at the prior time step to obtain an estimate of CARRT's current position.

This double integration is very sensitive to errors in the acceleration measurement.  Several mechanism
are used to help manage those errors.

* To obtain the best acceleration measurment possible, each
accelerometer "measurement" actually is an average of 12 accelerometer measurements equally spaced in
time over the previous 1/8 second.

* The measured acceleration is filtered to remove near-zero noise. Small, near-zero accelerations are replaced
by 0 to ensure that "noise" isn't treated as actual acceleration (which can potentially over time integrate
into significant velocities).  The filter settings were determined empirically by measuring the noise in
accelerometer data when stationary.

* The estimated velocity obtained after integrating acceleration is constrained to what has been empirically
determined as CARRT's top speed.  This prevent the integration process from producing "run-away" velocities.

* When CARRT stops, it tells the the Navigator (via a function call), forcing velocity and acceleration to 0.
And no navigation updates occur while CARRT is stopped.

Desptie these steps, the velocity estimates occasionally spike high, maxing out velocity and excessively
over-estimating the distance traveled.  However, in most cases the inertial Navigator tends to underestimates
distance traveled.  The underestimate is generally 40% to 70% of actual distance traveled. The underestimates
cannot be reliably reproduced and seem to have a random component.  I have not been able to determine the
cause of these underestimates.  Nor for that matter, have I been able to determine the cause of the occasional
velocity spikes.

For these reasons, I fell back to an alternate navigation mode that implements dead-reckoning.


## Dead-Reckoning Navigation Mode

Dead-reckoning navigation follows the same first 4 steps, just like inertial navigation mode.  This includes taking
accelerometer measurements because the acceration vector is needed to determine the conversion from local to
global coordinates; acceration is not further used.

To compute distance traveled, distance traveled as a function of time was measured empirically and found to be an
excellent fit to a linear model.  This linear model is coded in DriveParam.cpp, with functions to compute both
distance given time and time given distance.

At each 1/8 second update, the dead-reckoning mode simply computes distance traveled for that 1/8 of a second and
applies it along CARRT's current heading vector to update the prior time-step position to the current position.

Dead-reckoing mode has some risks and error sources:

* Doesn't account for any slipage or slowing due to obstacles (e.g., getting over the lip of a rug).

* Doesn't account for drive speed dropping as motor battery levels drop. However, limited experimentation
indicates that CARRT drive speed stays pretty constant until some point at which it drops significantly and
rapidly.  So this problem can be minimized by driving with batteries that are "not too low".

* The linear approximation error is highest for short (< ~2 sec) drives.  But even then the error in the linear
model is much less than the errors observed from tnertial mode navigation.

All-in-all, the dead-reckoning navigation model appears to work well enough for autonomous driving over the kinds
of distances encountered during "inside the house" testing.

Nevertheless, it is frustrating that I couldn't get inertial navigation mode to work.
