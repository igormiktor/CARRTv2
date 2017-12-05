---
layout: home
#title: CARRT &mdash; an Autonomous Path-Finding Rover
#permalink: /
---

# CARRT v2

Goal of this project is to create a small robotic vehicle (CARRT) that can do the following:

- Start in one room of a house;
- Receive a goal position, specified relative to its starting location (e.g., 8 meters North, 4.5 meters West);
- Autonomously make its way to the new position, mapping doors and walls with its lidar until it finds a path to the goal;
- Build a map as it goes, enabling effective back-tracking whenever it hits a dead-end.

And do it all based on an 8 bit, 16 MHz ATmega 2560 microcontroller with only 8 KB SRAM and 256 KB program memory...
