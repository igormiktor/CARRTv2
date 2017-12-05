/*
    HelperStates.cpp - Helper states for CARRT states that need inputs

    Copyright (c) 2017 Igor Mikolic-Torreira.  All right reserved.

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




#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD || CARRT_INCLUDE_GOTODRIVE_IN_BUILD


#include "HelperStates.h"

#include <avr/pgmspace.h>

#include "ErrorCodes.h"
#include "EventManager.h"
#include "MainProcess.h"
#include "WelcomeMenuStates.h"

#include "Drivers/Display.h"
#include "Drivers/Keypad.h"





//********************************************************************


namespace
{
    const PROGMEM char sLabelYes[]  = "Yes";
    const PROGMEM char sLabelNo[]   = "No";
};


YesOrNoState::YesOrNoState( PGM_P title ) :
mTitle( title ),
mYes( 0 )
{
    // Nothing else
}


void YesOrNoState::onEntry()
{
    mYes = 0;
    Display::clear();
    Display::displayTopRowP16( mTitle );
    displayYesNo();
}


bool YesOrNoState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            // Got our answer
            State* newState;
            if ( mYes )
            {
                newState = onYes();
            }
            else
            {
                newState = onNo();
            }
            MainProcess::changeState( newState );
        }
        else
        {
            // Toggle the answer
            mYes = !mYes;
            displayYesNo();
        }
    }
    return true;
}


void YesOrNoState::displayYesNo()
{
    if ( mYes )
    {
        Display::displayBottomRowP16( sLabelYes );
    }
    else
    {
        Display::displayBottomRowP16( sLabelNo );
    }
}











//********************************************************************


EnterIntMenuState::EnterIntMenuState( PGM_P title, int min, int max, int inc, int initial ) :
mTitle( title ),
mMin( min ),
mMax( max ),
mInc( inc ),
mValue( initial )
{
    // Nothing else
}


void EnterIntMenuState::onEntry()
{
    Display::clear();
    Display::displayTopRowP16( mTitle );
    displayValue();
}


bool EnterIntMenuState::onEvent( uint8_t event, int16_t button )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        if ( button & Keypad::kButton_Select )
        {
            // Got our answer
            State* newState = onSelection( mValue );
            MainProcess::changeState( newState );
        }
        else if ( button & Keypad::kButton_Left || button & Keypad::kButton_Down )
        {
            // Decrement value
            mValue -= mInc;

            if ( mValue < mMin )
            {
                mValue = mMin;
            }

            displayValue();
        }
        else if ( button & Keypad::kButton_Right || button & Keypad::kButton_Up )
        {
            // Increment value
            mValue += mInc;

            if ( mValue > mMax )
            {
                mValue = mMax;
            }

            displayValue();
        }
    }
    return true;
}


void EnterIntMenuState::displayValue()
{
    Display::clearBottomRow();
    Display::setCursor( 1, 3 );
    Display::print( mValue );
}




#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD



