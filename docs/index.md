---
#
# Here you can change the text shown in the Home page before the Latest Posts section.
#
# Edit cayman-blog's home layout in _layouts instead if you wanna make some changes
# See: https://jekyllrb.com/docs/themes/#overriding-theme-defaults
#
layout: home
---


---
layout: home
#title: CARRT &mdash; an Autonomous Path-Finding Rover
#permalink: /
---

# CARRTv2
Cute Autonomous Robot Rover Thing (version 2)

Goal of this project is to create a small robotic vehicle (CARRT) that can do the following:

- Start in one room of the house;
- Receive a goal position, specified relative to its starting location (e.g., 8 meters North, 4.5 meters West);
- Autonomously make its way to the new position, mapping doors and walls with its ultrasonic radar until it finds a path to the goal;
- Build a map as it goes, enabling effective back-tracking whenever it hits a dead-end.

This is both a hardware and a software project.

The hardware consists of a four-wheeled chassis carrying an ArduinoMega 256 and an Adafruit motor driver board.  Navigation is
enabled by an LSM303DLHC three-axis accelerometer and magnetometer and an L3GD20 three-axis gryoscope.  A Garmin Lidar Lite
sensor mounted on a servo allows CARRT to sense obstacles (both furniture and walls) and navigate around them.

The hardware and how it is wired together is described in the schematics sub-directory by a
[Fritzing](http://fritzing.org/home/) project file.
Photos of CARRT appear in the images` sub-director and on the project wiki.

The software is found in the source directory.  Having the entire software
system fit and operate within the constraints of the ArduinoMega 256 (8 bit, 16
MHz ATmega 2560 microcontroller; 8 KB SRAM; 256 KB program memory) is the
primary challenge.  The rather low precision of the LSM303DLHC and the L3GD20 as
well as the poor reliability of a magnetometer operating inside a house (bring
up the compass on an iPhone, set the iPhone on the floor and just watch as you push
the iPhone slowly past an operating refrigerator) make indoor navigation the
secondary challenge.

The name "CARRT" is thanks to my daughter.

## Specs

* Main microcontroller
    * Arduino Mega256 board
    * CPU: ATmega2560 8-bit, 16MHz microcontroller
    * 256KB flash memory (program memory)
    * 8KB SRAM (working memory)
    * 4KB EEPROM (permanent memory)
* Power buses
    * Two power buses
    * First power bus supplies the Arduino Mega256
        * Arduino in turn supplies regulated 5V power to all other ICs
    * Second power bus supplies unregulated power to motors and servos
* Motor controllers
    * L293D H-Bridge ICs to control 4 DC motors
    * Total of 4 L293Ds are used
    * L293Ds control the unregulated motor power bus
    * L293Ds stacked in pairs to support 1.2A sustained current to DC motors
    * SN74H595 shift-register IC is used to control the L293Ds
    * SN74H595 works off the regulated 5V power bus
* 2 x 16 character LCD display
    * LCD controlled by an HD44780U
    * MCP23017 drives the HD44780U
* LSM303DLHC triple-axis accelerometer and magnetometer
* L3GD20 triple-axis gyroscope
* PCA9685 16-channel PWM/Servo driver
    * Used to drive the Parallax servo
* Parallax 900-00008 servo
* Garmin Lidar Lite v3
    * Mounted on the Parallax servo
* PING))) ultrasonic distance sensor
    * Used for obstacle detection while moving
* TMP36 analog temperature sensor
* PS1240 piezoelectric buzzer
