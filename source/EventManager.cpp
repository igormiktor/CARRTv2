/*
    EventManager.cpp - An event handling system for CARRT.

    Copyright (c) 2022 Igor Mikolic-Torreira.  All right reserved.

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





#define EVENTMANAGER_EVENT_QUEUE_SIZE		24


#include "EventManager.h"

#include <util/atomic.h>

#include "Utils/DebuggingMacros.h"



#if CARRT_ENABLE_EVENTMANAGER_DEBUG

#define EVTMGR_DEBUG_PRINT( x )             DEBUG_PRINT( x )
#define EVTMGR_DEBUG_PRINTLN( x )           DEBUG_PRINTLN( x )
#define EVTMGR_DEBUG_PRINT_P( x )           DEBUG_PRINT_P( x )
#define EVTMGR_DEBUG_PRINTLN_P( x )         DEBUG_PRINTLN_P( x )
#define EVTMGR_DEBUG_PRINT_PTR( x )         DEBUG_PRINT( reinterpret_cast<unsigned long>( x ), HEX );
#define EVTMGR_DEBUG_PRINTLN_PTR( x )       DEBUG_PRINTLN( reinterpret_cast<unsigned long>( x ), HEX );

#else

#define EVTMGR_DEBUG_PRINT( x )
#define EVTMGR_DEBUG_PRINTLN( x )
#define EVTMGR_DEBUG_PRINT_P( x )
#define EVTMGR_DEBUG_PRINTLN_P( x )
#define EVTMGR_DEBUG_PRINT_PTR( x )
#define EVTMGR_DEBUG_PRINTLN_PTR( x )

#endif



namespace EventManager
{

    // EventQueue class used internally by EventManager
    // cppcheck-suppress noConstructor
    class EventQueue
    {

    public:

        // Queue initializer (so we control when this happens)
        void init();

        // Reset (empty) the queue
        void reset();

        // Returns true if no events are in the queue
        bool isEmpty();

        // Returns true if no more events can be inserted into the queue
        bool isFull();

        // Actual number of events in queue
        uint8_t getNumEvents();

        // Tries to insert an event into the queue;
        // Returns false if successful, true if the queue if full and the event cannot be inserted
        //
        // NOTE: if EventManager is instantiated in interrupt safe mode, this function can be called
        // from interrupt handlers.  This is the ONLY EventManager function that can be called from
        // an interrupt.
        bool queueEvent( uint8_t eventCode, int16_t eventParam );

        // Tries to extract an event from the queue;
        // Returns true if successful, false if the queue is empty (the parameteres are not touched in this case)
        bool popEvent( uint8_t* eventCode, int16_t* eventParam );

    private:

        // Event queue size.
        // The maximum number of events the queue can hold is kEventQueueSize
        // Increasing this number will consume 3 bytes of RAM for each unit.
        static const uint8_t kEventQueueSize = EVENTMANAGER_EVENT_QUEUE_SIZE;

        struct EventElement
        {
            int16_t param;  // each event has a single integer parameter
            uint8_t code;   // each event is represented by an integer code
        };

        // The event queue
        EventElement mEventQueue[ kEventQueueSize ];

        // Index of event queue head
        uint8_t mEventQueueHead;

        // Index of event queue tail
        uint8_t mEventQueueTail;

        // Actual number of events in queue
        uint8_t mNumEvents;
    };


    EventQueue  mHighPriorityQueue;
    EventQueue  mLowPriorityQueue;

    uint8_t sQueueOverflowOccurred;

 };




//*********  INLINES   EventManager::EventQueue::  ***********

inline bool EventManager::EventQueue::isEmpty()
{
    return ( mNumEvents == 0 );
}


inline bool EventManager::EventQueue::isFull()
{
    return ( mNumEvents == kEventQueueSize );
}


inline uint8_t EventManager::EventQueue::getNumEvents()
{
    return mNumEvents;
}







uint8_t EventManager::getNextEvent( uint8_t* eventCode, int16_t* param )
{
    if ( mHighPriorityQueue.popEvent( eventCode, param ) )
    {
        return 1;
    }

    // If  there are no high-pri events try low-pri...
    if ( mLowPriorityQueue.popEvent( eventCode, param ) )
    {
        return 1;
    }

    return 0;
}



void EventManager::init()
{
    mHighPriorityQueue.init();
    mLowPriorityQueue.init();

    sQueueOverflowOccurred = false;
}



void EventManager::reset()
{
    mHighPriorityQueue.reset();
    mLowPriorityQueue.reset();

    sQueueOverflowOccurred = false;
}



bool EventManager::isEventQueueEmpty( EventPriority pri )
{
    return ( pri == kHighPriority ) ? mHighPriorityQueue.isEmpty() : mLowPriorityQueue.isEmpty();
}



bool EventManager::isEventQueueFull( EventPriority pri )
{
    return ( pri == kHighPriority ) ? mHighPriorityQueue.isFull() : mLowPriorityQueue.isFull();
}



uint8_t EventManager::getNumEventsInQueue( EventPriority pri )
{
    return ( pri == kHighPriority ) ? mHighPriorityQueue.getNumEvents() : mLowPriorityQueue.getNumEvents();
}



bool EventManager::queueEvent( uint8_t eventCode, int16_t eventParam, EventPriority pri )
{
    return ( pri == kHighPriority ) ?
        mHighPriorityQueue.queueEvent( eventCode, eventParam ) : mLowPriorityQueue.queueEvent( eventCode, eventParam );
}



bool EventManager::hasEventQueueOverflowed()
{
    return sQueueOverflowOccurred;
}



// cppcheck-suppress unusedFunction
void EventManager::resetEventQueueOverflowFlag()
{
    sQueueOverflowOccurred = false;
}




/******************************************************************************/




void EventManager::EventQueue::init()
{
    mEventQueueHead = 0;
    mEventQueueTail = 0;
    mNumEvents = 0;

    for ( uint8_t i = 0; i < kEventQueueSize; i++ )
    {
        mEventQueue[i].code = EventManager::kNullEvent;
        mEventQueue[i].param = 0;
    }
}



void EventManager::EventQueue::reset()
{
    mEventQueueHead = 0;
    mEventQueueTail = 0;
    mNumEvents = 0;
}



bool EventManager::EventQueue::queueEvent( uint8_t eventCode, int16_t eventParam )
{
    /*
    * The call to noInterrupts() MUST come BEFORE the full queue check.
    *
    * If the call to isFull() returns FALSE but an asynchronous interrupt queues
    * an event, making the queue full, before we finish inserting here, we will then
    * corrupt the queue (we'll add an event to an already full queue). So the entire
    * operation, from the call to isFull() to completing the inserting (if not full)
    * must be atomic.
    *
    * Note that this race condition can only arise IF both interrupt and non-interrupt (normal)
    * code add events to the queue.  If only normal code adds events, this can't happen
    * because then there are no asynchronous additions to the queue.  If only interrupt
    * handlers add events to the queue, this can't happen because further interrupts are
    * blocked while an interrupt handler is executing.  This race condition can only happen
    * when an event is added to the queue by normal (non-interrupt) code and simultaneously
    * an interrupt handler tries to add an event to the queue.  This is the case that the
    * cli() (= noInterrupts()) call protects against.
    *
    * Contrast this with the logic in popEvent().
    *
    */

    // Because this function may be called from interrupt handlers, debugging is
    // only available in when NOT in interrupt safe mode.


    bool retVal = true;         // true means an error occurred

    // ATOMIC BLOCK BEGIN
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        if ( !isFull() )
        {
            // Store the event at the tail of the queue
            mEventQueue[ mEventQueueTail ].code = eventCode;
            mEventQueue[ mEventQueueTail ].param = eventParam;

            // Update queue tail value
            mEventQueueTail = ( mEventQueueTail + 1 ) % kEventQueueSize;;

            // Update number of events in queue
            mNumEvents++;

            retVal = false;
        }
    }
    // ATOMIC BLOCK END

    if ( retVal )
    {
        EventManager::sQueueOverflowOccurred = true;
    }

    return retVal;
}




bool EventManager::EventQueue::popEvent( uint8_t* eventCode, int16_t* eventParam )
{
    /*
    * The call to noInterrupts() MUST come AFTER the empty queue check.
    *
    * There is no harm if the isEmpty() call returns an "incorrect" TRUE response because
    * an asynchronous interrupt queued an event after isEmpty() was called but before the
    * return is executed.  We'll pick up that asynchronously queued event the next time
    * popEvent() is called.
    *
    * If noInterrupts() is set before the isEmpty() check, we pretty much lock-up the Arduino.
    * This is because popEvent(), via processEvents(), is normally called inside loop(), which
    * means it is called VERY OFTEN.  Most of the time (>99%), the event queue will be empty.
    * But that means that we'll have interrupts turned off for a significant fraction of the
    * time.  We don't want to do that.  We only want interrupts turned off when we are
    * actually manipulating the queue.
    *
    * Contrast this with the logic in queueEvent().
    *
    */

    if ( isEmpty() )
    {
        return false;
    }

    // This function is NOT designed to called from interrupt handlers, so
    // it is safe to turn interrupts on instead of restoring a previous state.
    // ATOMIC BLOCK BEGIN
    ATOMIC_BLOCK( ATOMIC_FORCEON )
    {
        // Pop the event from the head of the queue
        // Store event code and event parameter into the user-supplied variables
        *eventCode  = mEventQueue[ mEventQueueHead ].code;
        *eventParam = mEventQueue[ mEventQueueHead ].param;

        // Clear the event (paranoia)
        mEventQueue[ mEventQueueHead ].code = EventManager::kNullEvent;

        // Update the queue head value
        mEventQueueHead = ( mEventQueueHead + 1 ) % kEventQueueSize;

        // Update number of events in queue
        mNumEvents--;
    }

    EVTMGR_DEBUG_PRINT_P( PSTR( "popEvent() return " ) )
    EVTMGR_DEBUG_PRINT( *eventCode )
    EVTMGR_DEBUG_PRINT( ", " )
    EVTMGR_DEBUG_PRINTLN( *eventParam )

    return true;
}
