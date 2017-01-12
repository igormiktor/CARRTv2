# CARRT Software Design Notes


## Design Principles

This version of the software is based on the following design principles:

1. **Highly modular and decoupled.** Compartment details as much as
possible; present only the higher level interfaces needed by other
components, and no more.

2. **Separate low-level hardware interfaces from higher-level code.** Software
drivers are provided for all the hardware sub-systems, even trivial hardware
such as the piezo-electric buzzer. In keeping with principle #1, all details of
the hardware are encapsulated by the drivers.

3. **Prefer functional interfaces to object interfaces.** All the
hardware on CARRT is "one of".  This is true even for the four drive
motors which are all controlled through a single SN74H595 and so behave
as a single hardware subsystem.  If hardware drivers are presented as
objects, then the code has to manage a singleton object and make it
visible to code that needs to access it.  Functional interfaces as
easier and cleaner in this case, and avoid the proliferation of global
objects or the overhead of singleton classes to manage them.

4. **Use namespaces to compartmentalize modules.** Namespaces serve to
prevent name clashes and bind the functions that belong together
logically, much like using static class members. They also have a
simplified interface, as compared to classes with static members.

5. **Represent CARRT higher-level functions as an event-driven state
machine.**   Whatever CARRT is doing is represented as a state.
Changes in behavior are changes in state. States are narrowly
defined and simple.  More complex behaviors are decomposed into
simpler actions that correspond to simple, easily defined states.

6. **Represent states by classes.**  In this case, objects are the natural and
appropriate way to implement states.  Objects that inherit from a base class
allow simple and consistent management of states and a seamless process for
switching between states.

7. **States, and only states, are created dynamically.**  It may seem crazy
to use dynamic memory allocation on a microcontroller, but this principle is
based on actual testing that showed dynamic allocation offered the best
tradeoff.  If states are not allocated dynamically, then all states have to
allocated globally, which consumes a very significant amount of SRAM (states are
generally small, but there are many of them).  Because only one state is active
at a time, and no more than two are ever "live" simultaneously (when states are
changing both the outgoing and incoming states are briefly alive at the same
time).  So allocating states dynamically offers a significant reduction in
memory used at any instant in time.  Further supporting the use of dynamic
memory allocation for states are the following considerations:
    * The avr-libc implementations of `malloc()` and `free()` (upon
    which `new` and `delete` are implemented) are efficient and
    relatively fast.  And `free()` is pretty good at minimizing heap
    fragmentation.
    * The pattern of how states are allocated and released (create a new
    state; delete the old one; repeat) combined with the generally small
    size of states limits the fragmentation of the heap to at most a
    free block the size of the largest state.  And even that will be
    reclaimed on the very next state change because a new smaller state
    will then be created at the bottom of the heap (where the free
    fragment is) and then all the memory above it will be recombined
    into an un-fragmented free block when the old state (the state that
    had fragmented the heap) is released.

8. **Navigation is provided as a separate, stand-alone service.** The
Navigator is in essence a higher-level "driver" that is implemented on top of
the lower-level hardware drivers.

9. **All strings are placed in PROGMEM (flash memory).**  SRAM is
precious, and strings are needed only to support the user interface,
so string operations are never time critical.  While putting strings
in PROGMEM makes access much slower, the slower access doesn't matter
and the reduced SRAM usage is really important.

10. **Support selective compilation of major elements.**  CARRT is constrained
by SRAM. While it is useful to have a full set of testing code, it invariably
uses up precious SRAM.  So it is important to build the code in a way that
testing code (or other major elements of CARRT, such as the "programmed drive"
feature) can be omitted at compile time while still building a CARRT executable
that is otherwise fully operational apart from the omitted functionality



## History and Lessons

The initial implementation of CARRT software was developed in parallel with the
hardware build. There were many lesson learned on both sides. For example, the
original hardware hardware had only a single power bus for both the motors and
the electronic components.  Initial testing showed this to be a bad design: the
motors turning on caused large voltage drops that cause the ATmega 2560 to
reset.  That's why now there are separate power buses for motors and ICs.
Another example is that trying to drive a servo directly from the ATmega 2560
using PWM caused servo jitter due to the many other interrupts that the ATmega
2560 had to service.  This is why CARRT uses a dedicated PCA9685 board to
control the servo.

The initial software also avoided all use of dynamic memory allocation: all
states were pre-allocated as global objects.  As the number of states grew, this
quickly consumed much of available SRAM.  This was further exacerbated by
numerous user-interface strings that were stored in SRAM.  Each behavior (i.e.,
state) added to CARRT further compounded the problem.

Implementing software interfaces to hardware as objects created a
significant number of global objects that had to be managed and shared.
Eventually, this became a header file full of `extern` declarations of
global objects, which in turn had to include headers defining each of
these objects.  This became a headache to manage, slowed compilation,
and had the effect of making everything dependent on everything.

Additionally, some of the software interfaces were initially coded at the
wrong level of abstaction (generally too low a level).  This resulted in
what effectively became multiple layers of drivers (each somewhat higher
level) for a single (simple) piece of hardware.  This was needlessly
inefficient and hard to understand and debug.

Finally, the parallel development of hardware and software not only resulted in
a somewhat ad-hoc development of code, but also, to some extent, led to
undesirable coupling of testing code and operational code in some places and
code duplication in others.  This was inherently inefficient and it was hard to
adapt and maintain when new hardware was added and/or changes were need.

All these lessons (and numerous smaller ones) have been incorporated
into version 2 of the CARRT software.



## Implementation Overview

The implementation is at its core an event-driven state machine: the heart of
CARRT is an event loop that takes incoming events, feeds them to the current
state, which in turn takes appropriate actions (including triggering a change to
a new state).

The top level view of current CARRT implementation is as
follows:

* Initialization.  Configure the microcontroller appropriately, initialize all
hardware subsystems, initialize the state machine, and start internal timers.

* Event loop. This is the heart of CARRT.  The main loop is very simple: pull
the next event from the even queue, handle any system-level events (navigation
updates, errors, resets, etc), pass any other events to the current state (this
is where all the behaviors happen), and then (when the state is done with the
event) optionally perform (non-essential)  housekeeping tasks.

* Reset. If a reset is requested, ensure everything is shutdown appropriately,
clean up, and then go back to initialization.


Initialization and reset is handled in CarrtMain.cpp; the event loop is managed
in MainProcess.cpp.  State behavior is defined by the the State class (State.h
and State.cpp).  States have a simple interface of three member functions:
onEntry(), onEvent(), and onExit().  When a state becomes active (i.e.,
current), but before any events are processed, MainProcess will call the state's
onEntry() function.  This provides a mechanism for the state to initialize or
otherwise prepare itself.  The MainProcess will then repeatedly pass events to
the state's onEvent() function.  It will continue to do so until there is a
change of state (usually, but not always, initiated by the state's onEvent()
function).  When a change of state is triggered, the MainProcess will then call
the the state's onExit() function, allowing the state to perform whatever clean
up is appropriate.  In most cases, the last thing the state's onExit() will do
is delete itself; this is the default action of State::onExit(). In some
situations (e.g, for states that are part of a user-entered program of actions
which the user may replay repeatedly) states will override the self-delete
behavior in onExit().

Events are defined and managed by the EventManager (EventManager.h,
EventManager.cpp).  At this time, events consist of timer-related events,
navigation-related events, keypad-related events, and error events.  The
timer-related events are the most important, as they provide the heart-beat that
enables States to preform a variety of actions, such as monitoring drive
progress or navigation parameters, updating displays, tracking elapsed time, and
initiating time related actions.  Timer-related events are generated by the
EventClock (EventClock.h and EventClock.cpp), which relies on one of the ATmega
2560's hardware timers to trigger interrupts that are used to queue
timer-events. Note also that changes of state are not events, but rather
something that happens as a result of an event.

The inertial navigator (Navigator.h and Navigator.cpp) is implemented as an
independent module that effectively runs in the background.  The MainProcess
feeds it dedicated navigation update events every eighth of a second, which the
Navigtor uses to read accelerometer, magnetometer, and gyroscope data and update
its internal navigation state.  At any time, States can call appropriate
Navigator functions to obtain the current position, direction, and velocity.  To
help the Navigator out, States inform the Navigator about what CARRT is doing
(e.g., driving straight, turning, stopped, etc) by calling appropriate Navigator
functions (e.g., Navigator::movingStraight(), Navigator::movingTurning(),
Navigator::stopped(), etc).
