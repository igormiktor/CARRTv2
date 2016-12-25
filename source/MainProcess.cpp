/*
    MainProcess.cpp - Functions that run CARRT's main process which manages
    CARRT's states

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





#include "MainProcess.h"

#include "AVRTools/SystemClock.h"
#include "AVRTools/MemUtils.h"

#include "State.h"
#include "MenuStates.h"
#include "ErrorCodes.h"
#include "ErrorState.h"
#include "EventManager.h"

#include "Drivers/Battery.h"
#include "Drivers/Keypad.h"

#include "Utils/DebuggingMacros.h"



#ifndef CARRT_MIN_MEMORY
#define CARRT_MIN_MEMORY        64      // bytes
#endif




// Expand the namespace with some additional functionality private to this module

namespace MainProcess
{
    void checkForErrors();
    void processEvent();
    bool checkForUserInputs();
    void prepReset();
    bool handleRequiredSystemEvents( uint8_t event, int parameter );
    void handleOptionalSystemEvents( uint8_t event, int parameter );

    State*          mState;
    ErrorState*     mErrorState;
    bool            mNotPaused;
};






void MainProcess::init( ErrorState* errorState )
{
    EventManager::reset();

    mNotPaused = true;

    // Error state is special -- we hold it here for the duration
    // Low on memory is one of the error states, so we don't want to have to create
    // an error state on the fly
    mErrorState = errorState;

    mState = new WelcomeState;
    mState->onEntry();
}




void MainProcess::runEventLoop()
{
    while ( 1 )
    {
        checkForErrors();

        processEvent();

        if ( checkForUserInputs() )
        {
            // Reset triggered -- get out of the event loop
            break;
        }
    }
}





bool MainProcess::checkForUserInputs()
{
    const int kMinTimeBetweenButtonChecks = 250;        // milliseconds

    static unsigned long sNextTimeButtonClickAccepted = 0;

    uint8_t buttonHit = Keypad::readButtons();

    if ( buttonHit && millis() > sNextTimeButtonClickAccepted )
    {
        // Reset requested
        if ( buttonHit == Keypad::kChord_Reset )
        {
            prepReset();

            // Make sure we accept button clicks next time around
            sNextTimeButtonClickAccepted = 0;
            return true;
        }

        // Pause requested
        if ( buttonHit == Keypad::kChord_Pause )
        {
            // Set paused flag (we simply don't pass any events to the state when paused)
            mNotPaused = false;
            return false;
        }

        // Continue requested
        if ( buttonHit == Keypad::kChord_Continue )
        {
            // Clear paused flag
            mNotPaused = true;
            return false;
        }

        EventManager::queueEvent( EventManager::kKeypadButtonHitEvent, buttonHit );

        // Rollover happens in about 50 days, so don't worry about it
        sNextTimeButtonClickAccepted = millis() + kMinTimeBetweenButtonChecks;
    }

    return false;
}





void MainProcess::prepReset()
{
    // Give the state a chance to delete itself
    mState->onExit();
    mState = 0;
}





void MainProcess::processEvent()
{
    uint8_t eventCode;
    int16_t eventParam;

    if ( EventManager::getNextEvent( &eventCode, &eventParam ) )
    {
        // We have an event to process -- start with required system events
        if ( handleRequiredSystemEvents( eventCode, eventParam ) )
        {
            //  If returned true, (and not paused) give the current state a chance at the event
            if ( mNotPaused && mState->onEvent( eventCode, eventParam ) )
            {
                // If the state returned true, pass the event back to the system
                handleOptionalSystemEvents( eventCode, eventParam );
            }
        }
    }
}






void MainProcess::checkForErrors()
{
    if ( MemUtils::freeRamQuickEstimate() < CARRT_MIN_MEMORY )
    {
        setErrorState( kOutOfMemoryError );
    }
    else if ( EventManager::hasEventQueueOverflowed() )
    {
        setErrorState( kEventQueueOverflowError );
    }
}




void MainProcess::yield( uint16_t millisecs )
{
    // Allows other event processing to occur while we pause for a while

    // Does NOT process state events (so safe to call from inside a state)

    uint8_t eventCode;
    int16_t eventParam;

    // No rollover problem for many days...
    unsigned long endTime = millis() + millisecs;

    while ( millis() < endTime )
    {
        if ( EventManager::getNextEvent( &eventCode, &eventParam ) )
        {
            // We have an event to process -- start with required system events
            if ( handleRequiredSystemEvents( eventCode, eventParam ) )
            {
                //  If returned true, give the optional system handlers a chance at the event
                handleOptionalSystemEvents( eventCode, eventParam );
            }
        }
    }
}





void MainProcess::changeState( State* newState )
{
    // Only change if there really is a change
    if ( newState != mState )
    {
        // Design relies on states to delete themselves (if appropriate)
        mState->onExit();

        mState = newState;

        mState->onEntry();
    }
}





bool MainProcess::handleRequiredSystemEvents( uint8_t eventCode, int eventParam )
{
    if ( eventCode == EventManager::kNavUpdateEvent )
    {
        // gNavigator.doNavUpdate();
        return false;
    }

    if ( eventCode == EventManager::kNavDriftCorrectionEvent )
    {
        // gNavigator.doDriftCorrection();
        return false;
    }

    if ( eventCode == EventManager::kErrorEvent )
    {
        setErrorState( eventParam );
        return false;
    }

    return true;
}




void MainProcess::handleOptionalSystemEvents( uint8_t eventCode, int parameter )
{
    if ( eventCode == EventManager::kEightSecondTimerEvent )
    {
        int batteryStatus = Battery::checkAndDisplayBatteryStatus();
        if ( batteryStatus != Battery::kBatteriesOkay )
        {
//          postErrorEvent( (batteryStatus == kMotorBatteryCriticalLow ? kMotorBatteryLowError : kCpuBatteryLowError ) );
        }
    }
}




void MainProcess::postErrorEvent( int errorCode )
{
    EventManager::queueEvent( EventManager::kErrorEvent, errorCode, EventManager::kHighPriority );
}




void MainProcess::setErrorState( int errorCode )
{
    mErrorState->setErrorCode( errorCode );
    changeState( mErrorState );
}


