/*
    MainProcess.h - Functions that run CARRT's main process which manages
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






#ifndef StateMachine_h
#define StateMachine_h

#include <stdint.h>


class State;
class ErrorState;

namespace MainProcess
{
    void init( ErrorState* errorState );
    void yield( uint16_t milliseconds );
    void runEventLoop();
    void changeState( State* newState );
    void postErrorEvent( int errorCode );
    void setErrorState( int errorCode );
};



#endif
