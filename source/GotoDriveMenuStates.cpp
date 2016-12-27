/*
    GotoDriveMenuStates.cpp - Goto Drive Menu States for CARRT

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




#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD



#include "GotoDriveMenuStates.h"

#include <avr/pgmspace.h>

#include "EventManager.h"
#include "MainProcess.h"
#include "WelcomeMenuStates.h"

#include "Drivers/Display.h"




void GotoDriveState::onEntry()
{
    Display::displayTopRowP16( PSTR( "GoTo Drive Menu" ) );
    Display::displayBottomRowP16( PSTR( "Forthcoming..."  ) );
}


bool GotoDriveState::onEvent( uint8_t event, int16_t param )
{
    if ( event == EventManager::kKeypadButtonHitEvent )
    {
        MainProcess::changeState( new WelcomeState );
    }
}




#endif  // CARRT_INCLUDE_GOTODRIVE_IN_BUILD
