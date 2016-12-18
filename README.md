# CARRTv2
Cute Autonomous Robot Rover Thing (version 2)

Goal of this project is to create a small robotic vehicle (CARRT) that can do the following:

- Start in one room of the house
- Receive a goal position, specified relative to its starting location (e.g., 10 meters North, 8.5 meters West)
- Autonomously make its way to the new position, mapping doors and walls with its ultrasonic radar until it finds a path to the goal
- Build a map as it goes, enabling effective back-tracking whenever it hits a dead-end

This is both a hardware and a software project.   The hardware consists of a four-wheeled chassis carrying an ArduinoMega 256 and
an Adafruit motor driver board.  The navigation is enabled by an LSM303DLHC three-axis accelerometer and magnetometer and an
L3G20 gryoscope.  A Parallax ultrasonic ping sensor mounted on a servo allows CARRT to sense obstacles (both furniture and walls).

The hardware and how it is wired together is described in the `schematics` sub-directory by a [Fritzing] (http://fritzing.org/home/) project file.

The software is found in the `source` directory.

