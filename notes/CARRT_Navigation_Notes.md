# CARRT Navigation Notes


## Overview

CARRT can be compiled with one of two different navigation modes. The first mode
is inertial navigation. It uses the accelerometer, magnetometer and gyroscope as
an inertial measurement unit, estimating distance moved by (double)
integration of the acceleration.  The second mode is dead reckoning.  This mode
still uses the accelerometer, magnetometer, and gyroscope for orientation only,
but estimates distance moved using an empirically-derived formula that
converts time spent moving into distance moved.  The mode is chosen at compile time by
#defining either CARRT_NAVIGATE_USING_INERTIAL or
CARRT_NAVIGATE_USING_DEADRECKONING.


## Common to both Navigation Modes

The overall approach to navigation, the underlying choices of coordinate systems, and the implementation of
these fundamental elements are common to both navigation modes.  These common elements are:

* Coordinate system: The global coordinate system used by CARRT for navigation places North along the
positive X-axis and West along the positive Y-axis.  This allows the 0 direction to map to North and
enables the standard conversions between Cartesian (x,y) coordinates and Polar (r,&theta;) coordinates.
All navigation and navigation-related activities, such as mapping, are done in the global (N,W) coordinate
system.

* Local coordinates: The accelerometer and magnetometer both provide data in local, CARRT-centric,
3-D coordinates in which the X-axis points in CARRT's forward direction, the Y-axis points
to CARRT's left, and the Z-axis points upward.  The gyroscope provides angular rates along these same
three local axes. The conversion from local coordinates to global coordinates is dynamic, changing
over time as CARRT's orientation in 3-space changes (of course, orientation in the horizontal plane is
the primary consideration because CARRT usually doesn't drive on slopes&mdash;but the conversion
also works in that case).

* Angles: Navigation internally uses standard mathematical angles in radians, measured counter-clockwise
from the X-axis (North-axis). Conversion to compass angles is straightforward:

    compass = (360 - &theta; * 180 / pi) modulo 360

* Units: The units for the global (N,W) coordinates are internally maintained in meters (m).  Velocity is in
m/s, and acceleration is in m/s^2.  For convenience, centimeters (cm) units are commonly used in displays and
for keypad entry.

* Update rate: Navigation parameters are updated every 1/8 second.  The update reads all three instruments
(accelerometer, magnetometer, and gyroscope) and updates the orientation and position of CARRT.


Both navigation modes execute the following overall process:

1. Read the accelerometer, magnetometer, and gyroscope.

2. Estimate CARRT's current compass heading from the accelerometer and magnetometer data.  This step
also establishes the coordinate transformation between the local and global coordinate systems.

3. Estimate CARRT's current compass heading by integrating the gyroscope's angular rate data.

4. Combine the two compass estimates to get a "best" estimate for CARRT's current compass heading.

5. Using a velocity estimate (obtained differently for the different navigation modes), estimate
the distance traveled and update the global (N,W) coordinates of CARRT's current position.


The coordinate transformation figured out in step 2 is essential to all other
computations because all measurement data is in 3-D local coordinates but
must ultimately be converted to the 2-D global (N,W)
coordinate system. It isn't immediately obvious how to get to the global
coordinate system from the accelerometer, magnetometer, and gryoscope
measurments which are reported  as 3-D vectors in local coordinates.  The key to
converting from local to global coordinates is the observation that the cross
product of the magnetometer and the accelerometer output vectors gives an East
vector expressed in local coordinates.  The magnetometer output is a North
vector expressed in local coordinates, and these two vectors (East and North)
provide an orthogonal basis vector set for the global coordinate system expressed
in local coordinates.  With this basis set in hand, the local-to-global conversion
is straightforward.


## Inertial Navigation Mode

In inertial navigation mode the first 4 steps are executed as described above.
To implement step 5, the acceleration vector from the accelerometer is first
converted from local to global (N,W) coordinates. It is then integrated and
added to the velocity from the prior time-step to produce an estimate of the
current velocity vector.  A second integration is then performed on the current
velocity to get distance travelled and the result is added the position at the
prior time-step.  This generates an estimate of CARRT's current position.

The double integration required in intertial navigation mode is very sensitive
to errors in the acceleration measurement.  Several mechanism are used to help
limit those errors:

* To obtain the best acceleration measurement possible, each accelerometer "measurement" actually is an
average of 12 accelerometer measurements equally spaced in time over the previous 1/8 second.

* The measured acceleration is filtered to remove near-zero values. These small, near-zero accelerations are
replaced by 0 to ensure that "noise" isn't treated as actual acceleration (which if repeatedly integrated
can generate significant velocities).  The filter settings were determined empirically by analyzing the
noise in accelerometer measurements while CARRT was stationary.

* The estimated velocity obtained after integrating acceleration is constrained to what has been empirically
determined as CARRT's top speed.  This prevents the integration process from producing unreasonable velocities.

* When CARRT stops, velocity and acceleration are forced to 0 (regardless of what the accelerometer or other
instruments report).  And no navigation updates occur while CARRT is stopped.

Despite these steps, the velocity estimates occasionally spike high, maxing out velocity and excessively
over-estimating the distance traveled.  However, in most cases inertial navigation mode tends to underestimate
distance traveled.  The underestimate is generally 40% to 70% of actual distance traveled. The amount
underestimated cannot be reliably reproduced and seems to have a random component.  I have not been able to
determine the cause of these underestimates.  Nor, for that matter, have I been able to determine the cause of
the occasional velocity spikes.

For these reasons, I implemented an alternate navigation mode that implements dead-reckoning.


## Dead-Reckoning Navigation Mode

The implementation of dead-reckoning navigation mode follows the same first 4 steps, just like inertial
navigation mode.  This includes taking accelerometer measurements because the acceration vector is needed
to compute heading and determine the conversion from local to global coordinates.  In dead-reckoning mode,
this is the only use of  acceleration data.

To compute distance traveled, dead-reckoning mode uses a simple linear model of distance traveled as a
function of time.  This model was derived from empirical measurements of how far CARRT traveled over time. The
data showed that a linear model was a suprisingly good fit. This linear model is documented and implemented
in DriveParam.cpp, which includes functions to compute both distance given time and time given distance.

So, at each 1/8 second update,  dead-reckoning navigation mode simply uses the linear model to
compute how far CARRT traveled during that 1/8 of a second and applies it along CARRT's current heading
vector to update the prior time-step position to the current position.

Dead-reckoing mode has some risks and error sources:

* Doesn't account for slippage or slowing due to obstacles (e.g., getting over the lip of a rug).

* Doesn't account for drive speed dropping as motor battery levels drop. However, limited experimentation
indicates that CARRT drive speed stays pretty constant until some point at which it drops significantly and
rapidly.  So this problem can be minimized by driving with batteries that are "not too low".

* The linear approximation error is highest for short (< ~2 sec) drives.  But even then the error in the linear
model is much less than the errors observed from tnertial mode navigation.  Note that this applies to actual
drive times&mdash;not 1/8 second intervals of a longer drive.  Because of the linear model, adding up 1/8 second
intervals to compute distance traveled during a 30-second drive produces the same answer as computing the
distance traveled for a 30-second drive. The statement here applies to an actual drive: the estimate for a 30-second
drive (regardless of how it is computed) will be pretty accurate; the estimate for a 1/2 second drive (however computed)
will be much less accurate.

All-in-all, the dead-reckoning navigation model appears to work well enough for autonomous driving over the kinds
of distances and conditions encountered during "inside the house" driving that CARRT was designed for.

Nevertheless, it is frustrating that I couldn't get inertial navigation mode to work...
