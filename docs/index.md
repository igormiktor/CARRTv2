---
layout: home
#title: CARRT &mdash; an Autonomous Path-Finding Rover
#permalink: /
---

# CARRT v2

Goal of this project is to create a small robotic vehicle (CARRT) that can do the following:

- Start in one room of a house;
- Receive a goal position relative to its current position;
- Autonomously make its way to the goal position, mapping doors and walls with its lidar until it finds a path to the goal;
- Build a map as it goes, enabling effective back-tracking whenever it hits a dead-end.

And do it all based on an 8 bit, 16 MHz microcontroller with only 8 KB of SRAM...
