# CARRT Software Design Notes


## Design Principles

This version of the software is based on the following design principles:

1. **Highly modular and decoupled.** Compartment details as much as
possible; present only the higher level interfaces needed by other
components, and no more.

2. **Separate low-level hardware interfaces from higher-level code.**
Software drivers for all the hardware sub-systems, even trivial hardware
such as the piezo-electric buzzer. In keeping with principle #1, all
details of the hardware (pin numbers, etc) are encapsulated by the
drivers.

3. **Prefer functional interfaces to object interfaces.** All the
hardware on CARRT is a "one of".  This is true even for the four drive
motors which are all controlled through a single SN74H595 and so behave
as a single hardware subsystem.  If hardware drivers are presented as
objects, then the code has to manage a singleton object and make it
visible to code that needs to access it.  Functional interfaces as
easier and cleaner in this case, and avoid the proliferation of global
objects or the overhead of singleton classes to manage them.

4. **Use namespaces to compartmentalize modules.** Namespaces serve to
prevent name clashes and bind the functions that belong together
logically much like using static class members. They also present a
simplified public interface, as compared to classes with static members.

5. **Represent CARRT higher-level functions as an event-driven state
machine.**   Whatever CARRT is doing should be represented as a state.
Changes in behavior are changes in state. States should be narrowly
defined and simple.  More complex behaviors should be decomposed into
simpler actions that correspond to simple, easily defined states.

6. **Represent states by classes.**  In this case, objects are the
natural way to implement states and allow simple and consistent
management of states and switching between them.

7. **States, and only states, will be created dynamically.**  It may
seem crazy to use dynamic memory allocation on a microcontroller, but
this decision is based on actual testing that showed it to offer the
best tradeoff.  If states are not allocated dynamically, then all states
have to allocated globally, which consumes a very significant amount of
SRAM (states are generally small, but there are many of them).  Because
only one state is active at a time, and no more than two are ever "live"
simultaneously (when states are changing both the outgoing and incoming
states are briefly live at the same time).  So allocating states
dynamically offers a significant reduction in memory used at any instant
in time.  Further supporting the use of dynamic memory allocation for
states are the following considerations:
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
    into an un-fragmented free block when the old state (that fragmented
    the heap) is released.

8. **Navigation will be provided as a separate, stand-alone service** The Navigator is
in essence a higher-level "driver" that is implemented on top of the lower-level
hardware drivers.

9. **All strings will be placed in PROGMEM (flash memory)**  SRAM is
precious, and strings are needed only to support the user interface UI,
so string operations are never time critical.  So while putting strings
in PROGMEM makes access much slower, the slower access doesn't matter
and the reduced SRAM usage is really important.

10. **Support selective compilation of major elements**  CARRT is
invariably SRAM limited. While it is useful to have a full set of
testing code, it invariably uses up precious SRAM.  So it is important
to build in the code in a way that testing code (or other major elements
of CARRT, such as the "programmed drive" feature) can be omitted at
compile time while leaving a fully operational CARRT



## History and Lessons

The initial implementation of CARRT software was developed in parallel
with the hardware build. There were many lesson learned on both sides.
For example, the original hardware hardware had only a single power bus
for both the motors and the electronic components.  Initial testing
showed this to be a bad design: the motors turning on caused large
voltage drops that cause the ATmega 2560 to reset.  That's why there are
separate power buses for motors and ICs. Another example is that trying
to drive a servo directly from the ATmega 2560 using PWM caused servo
jitter due to the many other interrupts that the ATmega 2560 had to
service.  This is why CARRT uses a dedicated PCA9685 board to control
the servo.

The initial software also avoided all use of dynamic memory allocation:
all states were pre-allocated as global objects.  This quickly consumed
much of available SRAM.  This was further exacerbated by numerous UI
strings that were stored in SRAM.  Each behavior (i.e., state) added
further compounded the SRAM shortage.

Implementing software interfaces to hardware as objects created a
significant number of global objects that had to be managed and shared.
Eventually, this became a header file full of `extern` declarations of
global objects, which in turn had to include headers defining each of
these objects.  This became a headache to manage, slowed compilation,
and had the effect of making everything dependent on everything.

Additionally, some of the software interfaces where initially coded at the
wrong level of abstaction (generally too low a level).  This resulted in
what effectively became multiple layers of drivers (each somewhat higher
level) for a single (simple) piece of hardware.  This was needlessly
inefficient and hard to understand and debug.

Finally, the parallel development of hardware and software led to a
somewhat ad-hoc development of code, but also led to so extent to an
undesirable mixing and coupling of testing code and operational code in
some places and code duplication in others.  This was inherently
inefficient and hard to adapt and maintain as new hardware was added
and/or changes were need.

All these lessons (and numerous smaller ones) have been incorporated
into version 2 of the CARRT software.



## Implementation Overview

The implementation is at its core an event-driven state machine: the heart of
CARRT is an event loop that takes incoming events, feeds them to the current state,
which in turn takes appropriate actions, including triggering a change to a new state.

More specifically, the top level view of current CARRT implmentation is as follows:

* The microcontroller is appropriately configured


