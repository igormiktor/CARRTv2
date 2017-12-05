---
layout: post
title:  "Pathfinder working on CARRT"
date:   2017-12-03 10:09:18 -0500
categories: CARRT update pathfinder
---

The pathfinder is now implemented and tested on CARRT itself.  The original pathfinding code was developed and tested on Linux.
It's now working on CARRT's 8-bit, AVR processor with only 8K of SRAM.

It's been tested both with "manually" entered maps (PathFindingBasicTest.cpp) and with lidar-generated maps (PathFindingTest.cpp).

Next task is to create the menus and states that actually implement the pathfinding and then drive the resulting path (re-mapping and
re-pathfinding as appropriate along the way).

