/*
    ClockTest.cpp - Testing harness for the Rover Clock

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




#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/USART0.h"

#include "EventClock.h"
#include "EventManager.h"



int main()
{
    initSystem();
    initSystemClock();

    EventClock::init();

    Serial0 out;
    out.start( 115200 );

    delayMilliseconds( 1000 );

    out.println( "Clock test...\n" );

    uint8_t n = 0;

    out.println( "Sequence test\n" );

    // Check we get right sequence of events
    EventManager::reset();
    while ( n < 3 )
    {
        uint8_t event;
        int16_t param;

        if ( EventManager::getNextEvent( &event, &param ) )
        {
            switch( event )
            {
                case EventManager::kNavUpdateEvent:
                    out.print( "N, " );
                    break;

                case EventManager::kQuarterSecondTimerEvent:
                    out.print( "Q, " );
                    break;

                case EventManager::kOneSecondTimerEvent:
                    out.println( "S" );
                    break;

                case EventManager::kEightSecondTimerEvent:
                    out.println( "8\n" );
                    ++n;
                    break;

                default:
                    out.println( "\n** Other **\n" );
                    break;
            }
        }

        // This makes it very unlikely that we'll ever post RoverClock events inbetween polling for a hi-pri event and a low-pri event.
        // Without this, we poll for events so often that we frequently post RoverClock events in the middle of the polling for events.
        // This manifests by the quarter-second events appearing "before" the corresponding eighth-second events because the RoverClock
        // ISR fires between the check of the hi-pri queue and the low-pri queue in EventManager::getNextEvent().
        // It can still happen, but with the delay, the EventManager::getNextEvent() call happens rarely "enough" that the situation is
        // very low probability.
        // Without this delay, the above loop is essentially reduced to continuously calling EventManager::getNextEvent().

        delayMilliseconds( 25 );
    }


    // Test timing (against a stop watch)

    out.println( "\nTiming test\n" );

    n = 0;
    EventManager::reset();
    while ( 1 )
    {
        uint8_t event;
        int16_t param;

        if ( EventManager::getNextEvent( &event, &param ) )
        {
            switch( event )
            {
                case EventManager::kOneSecondTimerEvent:
                    out.println( n++ );
                    break;
            }
        }
    }

}

