# CARRT User Interface Overview


## Keypad
```
+-----------------------------------------+
| +-------------------------------------+ |
| |                 LCD                 | |
| +-------------------------------------+ |
|        1                                |
|     2     3    5   <--- Keypad Buttons  |
|        4                                |
+-----------------------------------------+
```

The diagram above identifies the buttons on the LCD keypad.  These are
referred to as follows:

* button 1 = UP
* button 2 = LEFT
* button 3 = RIGHT
* button 4 = DOWN
* button 5 = SELECT

It is also possible to enter simultaneous combinations of buttons, referred to as "chords".
Several chords have unchanging meaning.  These chords are:

* button 3 & button 5 = RIGHT & SELECT = RESET
* button 2 & button 3 = LEFT & RIGHT  = PAUSE
* button 1 & button 4 = UP & DOWN     = CONTINUE

* RESET always performs a full soft reset of CARRT.  All systems and sub-systems are
completely reinitialized.  It is effectively equivalent to cycling the power.

* PAUSE suspends the current activity in the sense that no further
state-level event processing (and therefore no further state-level
actions) will occur.  Note that "pause" can in some cases be
counterintuitive:  pausing a "forward drive" state while it is driving
will effectively cause CARRT to continue to drive indefinitely: the
events that trigger driving to stop will not be processed.

* CONTINUE will "undo" a PAUSE, allowing the active state to continue
processing events and taking corresponding action.  Note that "continue"
can in some cases be counterintuitive:  continuing a paused "forward
drive" state while it is driving will may not cause it to stop: if the
events that trigger driving to stop have already gone by, then the state
will never see them and will therefore not stop.

Apart from the above chords, the meaning of the buttons varies with menu items,
although the operation is generally intuitive.  The precise meanings, especially
any that are not perhaps intuitive, are described in the appropriate menu
sections.


## Menus

The menu system has a consistent user interface:  the UP and RIGHT buttons move
to the next menu item; the DOWN and LEFT buttons move to the previous menu item;
the SELECT button selects that menu item.

The remaining sub-sections walk through the menu options

### Welcome to CARRT (top-level menu)

This is the top-most menu in CARRT.  It presents four menu options:

* **Run Tests...** Leads to a menu of test options.

* **Prgm Drive...** Leads to menus that allow users to program a drive and run
it.

* **GoTo Drive...** Leads to menus that allow users to enter a destination and
have CARRT go to it.

* **Credits...** Displays credits.

Note that the first three options are present only if they are selected at
compile time.


### Run Tests... (second-level menu)

This menu presents a series of tests.  Selecting one of them runs the corresponding test.
The options under this menu are:

* **Exit...**  Returns to the top-level menu

* **Memory** Displays memory information.  The UP and DOWN buttons toggle
between displaying total free SRAM and total used SRAM. The LEFT and RIGHT
buttons toggle between displaying total SRAM and displaying the SRAM between the
stack and the heap (the "free stack" space).  The SELECT button returns to the **Run
Tests...** menu.

* **1/4 Sec Events**  Counts 1/4 second events.  Continues until any button is
pressed, which returns to the **Run Tests...** menu.

* **1 Sec Events**  Counts 1 second events.  Continues until any button is
pressed, which returns to the **Run Tests...** menu.

* **8 Sec Events**  Counts 8 second events.  Continues until any button is
pressed, which returns to the **Run Tests...** menu.

* **Beep**  Makes beep sounds until any button is pressed, which returns to the
**Run Tests...** menu.

* **Temp Sensor**  Displays the temperature as measured by the TMP36 analog
temperature sensor, updated every second.  Pressing any of the UP, DOWN, LEFT,
or RIGHT buttons toggles the display between degrees F and C.    The SELECT
button returns to the **Run Tests...** menu.

* **Batt LEDs**  Cycles through the battery level LEDs.  Continues until any
button is pressed, which returns to the **Run Tests...** menu.

* **Motor Batt**  Displays the voltage of the batteries that power the motors
and servos, updated every second.    Continues until any button is pressed,
which returns to the **Run Tests...** menu.

* **CPU Batt**    Displays the voltage of the batteries that power the electonic
components, updated every second.    Continues until any button is pressed,
which returns to the **Run Tests...** menu.

* **Sonar**  One a second the sonar measures and displays the range to whatever
is in front of it.  Continues until any button is pressed, which returns to the
**Run Tests...** menu.

* **Lidar**  The lidar slews to straight ahead, makes a range measurement, and
displays it.  Pressing the LEFT or RIGHT buttons slews the lidar accordingly.
Pressing the UP or DOWN button triggers a new lidar range measurement at the
current slew angle.  The SELECT button returns to the **Run Tests...** menu..

* **Range Scan**  The lidar performs a scan from right to left (covering a 120
degree arc), measuring and displaying the range to the nearest obstacle at 10
degree increments.  Every 4 seconds, the lidar slews 10 degrees to the new
bearing angle and displays the range to the nearest obstacle.  Continues until
any button is pressed, which returns to the **Run Tests...** menu.

* **Compass**  Displays the compass bearing in degrees, updated every
second.  Continues until any button is pressed, which returns to the
**Run Tests...** menu.

* **Accelerometer**  Displays the accelerometer readings in Gs, updated every
second.  The LEFT and RIGHT buttons cycle the display through the x-, y-, and
z-axes.  The UP and DOWN buttons toggle the display between raw accelerometer
readings and nulled accelerometer readings (nulled means the average at-rest
accelerometer reading is subtracted out).  The intent is that you can observe
the readings as CARRT is manually moved around.  The SELECT button returns to
the **Run Tests...** menu.

* **Gyroscope**  Displays the gyroscope readings in deg/sec, updated every
second.  The LEFT and RIGHT buttons cycle the display through the x-, y-, and
z-axes. The UP and DOWN buttons toggle the display between raw gryroscope
readings and nulled gryroscope readings (nulled means the average at-rest
gryroscope reading is subtracted out).  The intent is that you can observe the
readings as CARRT is manually spun around.  The SELECT button returns to the
**Run Tests...** menu.

* **Drive Fwd/Rev**  Repeatedly drives forward for a second, pauses for a
second, drives backward for a second, and pauses for a second.  Continues until
any button is pressed, which returns to the **Run Tests...** menu.

* **Drive Left/Right**  Repeatedly rotates left for a second, pauses for a
second, rotates right for a second, and pauses for a second.  Continues until
any button is pressed, which returns to the **Run Tests...** menu.

* **Nav. Rotation**  Engages the Navigator and displays which direction CARRT
believes it is pointed in (according to both compass and Navigator), updated
every second.  Continues until any button is pressed, which returns to
the **Run Tests...** menu.  The intent is that CARRT will be place on a
lazy-susan and spun around to check the performance of the Navigator.

* **Nav. Drive**  Engages the Navigator, drives forward for one second, and then
pauses to display how far (in cm) CARRT's Navigator believes it has traveled from
its starting point.  When any of the LEFT, RIGHT, UP, or DOWN buttons is pressed,
CARRT drives in reverse for one second, and again pauses to display how far CARRT's
Navigator thinks it is from its original starting point.  This continues until the
SELECT button returns to the **Run Tests...** menu. This allows checking CARRT's
internal estimates of distance traveled against actual measurements.

* **Error Handling**  Displays a short count-down and then triggers a error
condition, which (should) lead to the display of an error message (error code
601). The only recovery options are to perform a soft reset by simultaneously pressing
the RIGHT and SELECT buttons, or cycling the power.


### Prgm Drive... (second-level menu)

This set of menus allows the user to select from a set of actions and add them
to list of programmed CARRT actions.  Users can also clear the list or run the
list of actions (a "programmed drive").  The list is remembered (even if the
user exits the **Prgm Drive...** menu) until it is explicitly cleared.  The
number on the right
side of the top-row of the LED display shows the number of actions in the
currently stored "programmed drive".

The options under this menu are:

* **Exit...**  Returns to the top-level menu (the list of actions is
remembered).

* **Go Fwd Time**  Go forward for a user-specified number of seconds.

* **Go Fwd Dist**  Go forward for a user-specified distance in cm.

* **Go Rev Time**  Go in reverse for a user-specified number of seconds.

* **Go Rev Dist**  Go in reverse for a user-specified distance in cm.

* **Rotate L Time**  Rotate left (counter-clockwise) for a user-specified number
of seconds.

* **Rotate R Time**  Rotate right (clockwise) for a user-specified number of
seconds.

* **Rotate Angle**  Rotate a user-specified angle in degrees.  Negative angles
are to the right (clockwise); positive angles are to the left
(counter-clockwise).

* **Pause**  Pause (do nothing) for a user-specified number of seconds.

* **Beep**  Beep for a user-specified number of seconds.

* **Scan**  Conduct a range scan from -60 degrees relative to 60 degrees
relative, measuring and displaying the range to the nearest obstacle every 10
degrees.

* **Run it...**  Run the "programmed drive" using the currently stored actions.

* **Clear...**  Erase the currently stored actions.


### GoTo Drive... (second-level menu)

This set of menus allows the user to set a Go-To point and have CARRT
then autonomously drive to that Go-To point.  The Go-To point can be entered
either relative to CARRT's current orientation and position or in absolute
North and East coordiantes measured from CARRT's current position.

The options under this menu are:

* **Relative Goto...**  Specify a Go-To point relative to CARRT's current
orientation and position.  A series of prompts will allow the user to enter
the Go-To point as cross-range and down-range in centimeters (cm) from CARRT.

* **N & E Goto...**  Specify a Go-To point in North and East coordinates
measured from CARRT's current position.  A series of prompts will allow the
user to enter the Go-To point as distance (cm) to the North and East of CARRT.

* **Exit...**  Returns to the top-level menu.

