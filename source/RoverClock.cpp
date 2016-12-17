/*
    RoverClock.cpp - Runs CARRT's internal clock system.
    
    Copyright (c) 2016 Igor Mikolic-Torreira.  All right reserved.

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





#include "RoverClock.h"

#include "RoverEventManager.h"

#include <avr/interrupt.h>
#include <util/atomic.h>



// Select Timer2 or Timer5 to drive the Rover's internal clock

#if !defined( ROVER_CLOCK_USE_TIMER2 ) && !defined( ROVER_CLOCK_USE_TIMER5 ) 

#define ROVER_CLOCK_USE_TIMER5

#endif





#ifdef ROVER_CLOCK_USE_TIMER2


void initRoverClock()
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        // Clear Timer2 and configure it for Normal mode

        TCCR2A = 0;         // set entire TCCR2A register to 0
        TCCR2B = 0;         // same for TCCR2B
        TCNT2  = 0;         // initialize counter value to 0

        // Normal mode already set (bits WGM22, WGM21, and WGM20 cleared)

        // Set prescaler = 128 (set bits CS22 and CS20)
        TCCR2B |= (1 << CS22) | (1 << CS20);

        // enable timer overflow interrupt
        TIMSK2 |= (1 << TOIE2);
    }
}



ISR( TIMER2_OVF_vect )
{
    // Interrupts at 128 / 125,000 Hz = every 2.048 milliseconds.
    // 1/8 second = 61 interrupts + count only 9 on the last interrupt

    static uint8_t interruptCount = -61;
    static uint8_t eighthSecCount = 0;

    ++interruptCount;

    // Count up from -61 to make the comparison that happens most the time a comparison to zero
    if ( interruptCount == 0 )
    {
        // On 62nd cycle, only count 9, not the full 256
        TCNT2 = 256 - 9;
    }
    else if ( interruptCount == 1 )
    {
        // Hit an eighth second
        interruptCount = -61;       // Reset the count of interrupts

#if 0

        // Slower but more explicit implementation, retained as documentation of the logic

        ++eighthSecCount;
        eighthSecCount %= 64;

        // Queue nav update events every 1/8 second
        // Event parameter counts eighth seconds ( 0, 1, 2, 3, 4, 5, 6, 7 )
        EventManager::queueEvent( EventManager::kNavUpdateEvent, eighthSecCount % 8, EventManager::kHighPriority );

        if ( ( eighthSecCount % 2 ) == 0 )
        {
            // Event parameter counts quarter seconds ( 0, 1, 2, 3 )
            EventManager::queueEvent( EventManager::kQuarterSecondTimerEvent, (eighthSecCount % 8) / 2 );
        }

        if ( ( eighthSecCount % 8 ) == 0 )
        {
            // Event parameter counts seconds to 8 ( 0, 1, 2, 3, 4, 5, 6, 7 )
            EventManager::queueEvent( EventManager::kOneSecondTimerEvent, ( eighthSecCount / 8 ) );
        }

        if ( eighthSecCount == 0 )
        {
            EventManager::queueEvent( EventManager::kEightSecondTimerEvent, 0 );
        }

#else

        ++eighthSecCount;
        eighthSecCount &= 0x3F;                         // x &= 0x3F ==  x modulo 64

        // Queue nav update events every 1/8 second
        // Event parameter counts eighth seconds ( 0, 1, 2, 3, 4, 5, 6, 7 )
        EventManager::queueEvent( EventManager::kNavUpdateEvent, eighthSecCount & 0x07, EventManager::kHighPriority );

        if ( ( eighthSecCount & 0x01 ) == 0 )           // x & 0x01 == x modulo 2
        {
            // Event parameter counts quarter seconds ( 0, 1, 2, 3 )
            // Note:  (x & 0x07) >> 1 == (x % 8) / 2
            EventManager::queueEvent( EventManager::kQuarterSecondTimerEvent, (eighthSecCount & 0x07) >> 1 );
        }

        if ( ( eighthSecCount & 0x07 ) == 0 )           // x & 0x07 == x mod 8
        {
            // Event parameter counts seconds to 8 ( 0, 1, 2, 3, 4, 5, 6, 7 )
            // Note: (x >> 3) = x / 8
            EventManager::queueEvent( EventManager::kOneSecondTimerEvent, ( eighthSecCount >> 3 ) );
        }

        if ( eighthSecCount == 0 )
        {
            EventManager::queueEvent( EventManager::kEightSecondTimerEvent, 0 );
        }

#endif
        
        }
    }
}


#endif







#ifdef ROVER_CLOCK_USE_TIMER5


void initRoverClock()
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        // Set Timer5 for 8 Hz interrupts by using prescaler value 64 and
        // compare match register 31249.

        // Clear Timer5 and configure it for Compare Match mode

        TCCR5A = 0;         // set entire TCCR5A register to 0
        TCCR5B = 0;         // same for TCCR5B
        TCNT5  = 0;         // initialize counter value to 0

        // Set compare match register to desired timer count:
        // 31249 = 8 Hz
        OCR5A = 31249;

        // Clear CS52 and set CS51, CS50 bits for prescaler value: 64
        TCCR5B |= (1 << CS50);
        TCCR5B |= (1 << CS51);
        TCCR5B |= (0 << CS52);

        // Set CTC mode (bit WGM52 set, bits WGM51, and WGM50 cleared)
        TCCR5B |= (1 << WGM52);

        // enable timer compare interrupt:
        TIMSK5 |= (1 << OCIE5A);
    }
}



ISR( TIMER5_COMPA_vect )
{
    // Interrupts at 8 Hz = every 0.125 secs.

    static uint8_t eighthSecCount = 0;

#if 0

    // Slower but more explicit implementation, retained as documentation of the logic

    ++eighthSecCount;
    eighthSecCount %= 64;

    // Queue nav update events every 1/8 second
    // Event parameter counts eighth seconds ( 0, 1, 2, 3, 4, 5, 6, 7 )
    EventManager::queueEvent( EventManager::kNavUpdateEvent, eighthSecCount % 8, EventManager::kHighPriority );

    if ( ( eighthSecCount % 2 ) == 0 )
    {
        // Event parameter counts quarter seconds ( 0, 1, 2, 3 )
        EventManager::queueEvent( EventManager::kQuarterSecondTimerEvent, (eighthSecCount % 8) / 2 );
    }

    if ( ( eighthSecCount % 8 ) == 0 )
    {
        // Event parameter counts seconds to 8 ( 0, 1, 2, 3, 4, 5, 6, 7 )
        EventManager::queueEvent( EventManager::kOneSecondTimerEvent, ( eighthSecCount / 8 ) );
    }

    if ( eighthSecCount == 0 )
    {
        EventManager::queueEvent( EventManager::kEightSecondTimerEvent, 0 );
    }

#else

    ++eighthSecCount;
    eighthSecCount &= 0x3F;                         // x &= 0x3F ==  x modulo 64

    // Queue nav update events every 1/8 second
    // Event parameter counts eighth seconds ( 0, 1, 2, 3, 4, 5, 6, 7 )
    EventManager::queueEvent( EventManager::kNavUpdateEvent, eighthSecCount & 0x07, EventManager::kHighPriority );

    if ( ( eighthSecCount & 0x01 ) == 0 )           // x & 0x01 == x modulo 2
    {
        // Event parameter counts quarter seconds ( 0, 1, 2, 3 )
        // Note:  (x & 0x07) >> 1 == (x % 8) / 2
        EventManager::queueEvent( EventManager::kQuarterSecondTimerEvent, (eighthSecCount & 0x07) >> 1 );
    }

    if ( ( eighthSecCount & 0x07 ) == 0 )           // x & 0x07 == x mod 8
    {
        // Event parameter counts seconds to 8 ( 0, 1, 2, 3, 4, 5, 6, 7 )
        // Note: (x >> 3) = x / 8
        EventManager::queueEvent( EventManager::kOneSecondTimerEvent, ( eighthSecCount >> 3 ) );
    }

    if ( eighthSecCount == 0 )
    {
        EventManager::queueEvent( EventManager::kEightSecondTimerEvent, 0 );
    }

#endif

}



#endif



