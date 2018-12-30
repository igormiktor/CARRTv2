/*
    EventManager.h - The event management module for CARRT

    Copyright (c) 2018 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/






#ifndef EventManager_h
#define EventManager_h

#include <stdint.h>



namespace EventManager
{

    enum
    {
        kNullEvent = 0,

        // Timer events
        kQuarterSecondTimerEvent,
        kOneSecondTimerEvent,
        kEightSecondTimerEvent,

        // Nav update event
        kNavUpdateEvent,
        kNavDriftCorrectionEvent,

        // Error event
        kErrorEvent,

        // Keypad events
        kKeypadButtonHitEvent,

        kLastEvent
    };


    // EventManager recognizes two kinds of events.  By default, events are
    // are queued as low priority, but these constants can be used to explicitly
    // set the priority when queueing events
    //
    // NOTE high priority events are always handled before any low priority events.
    enum EventPriority { kLowPriority, kHighPriority };


    void init();

    // Reset event manager by reseting (purging) queues and clearing overflow flag
    void reset();

    // Returns true if no events are in the queue
    bool isEventQueueEmpty( EventPriority pri = kLowPriority );

    // Returns true if no more events can be inserted into the queue
    bool isEventQueueFull( EventPriority pri = kLowPriority );

    // Actual number of events in queue
    uint8_t getNumEventsInQueue( EventPriority pri = kLowPriority );

    // Tries to insert an event into the queue;
    // returns true if successful, false if the
    // queue if full and the event cannot be inserted
    bool queueEvent( uint8_t eventCode, int16_t eventParam, EventPriority pri = kLowPriority );

    // This function returns the next event
    uint8_t getNextEvent( uint8_t* eventCode, int16_t* eventParam );

    // Has the event queue overflowed?
    bool hasEventQueueOverflowed();

    // Reset the event queue overflow flag
    void resetEventQueueOverflowFlag();

};


#endif
