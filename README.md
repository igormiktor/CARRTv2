# CARRTv2
Cute Autonomous Robot Rover Thing (version 2)

Goal of this project is to create a small robotic vehicle (CARRT) that can do the following:

- Start in one room of the house;
- Receive a goal position, specified relative to its starting location (e.g., 8 meters North, 4.5 meters West);
- Autonomously make its way to the new position, mapping doors and walls with its ultrasonic radar until it finds a path to the goal;
- Build a map as it goes, enabling effective back-tracking whenever it hits a dead-end.

This is both a hardware and a software project.

The hardware consists of a four-wheeled chassis carrying an ArduinoMega 256 and an Adafruit motor driver board.  Navigation is
enabled by an LSM303DLHC three-axis accelerometer and magnetometer and an L3GD20 three-axis gryoscope.  A Parallax ultrasonic ping
sensor mounted on a servo allows CARRT to sense obstacles (both furniture and walls) and navigate around them.

The hardware and how it is wired together is described in the `schematics` sub-directory by a [Fritzing] (http://fritzing.org/home/) project file.
Photos of CARRT appear in the `images` sub-director and on the project wiki.

The software is found in the `source` directory.  Having the entire software system fit and operate within the constraints of the
ArduinoMega 256 (8 bit, 16 MHz ATmega 2560 microcontroller; 8 KB SRAM; 256 KB program memory) is the primary challenge.  The rather low precision
of the LSM303DLHC and the L3GD20 as well as the poor reliability of a magnetometer operating inside a house (trying pushing an iPhone slowly past
an operating refrigerator) make indoor navigation a secondary challenge.

The name is thanks to my daughter.

