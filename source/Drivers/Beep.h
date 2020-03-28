/*
    Beep.h - Driver for CARRT's simple audio system

    Copyright (c) 2020 Igor Mikolic-Torreira.  All right reserved.

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


#ifndef Beep_h
#define Beep_h

namespace Beep
{

    const unsigned int  kBeepDefaultBeepTone        = 40;
    const unsigned int  kBeepDefaultBeepDuration    = 50;
    const unsigned int  kBeepDefaultChirpTone       = 90;
    const unsigned int  kBeepDefaultChirpDuration   = 20;

    const unsigned int  kBeepReadyTriTone1          = 0;
    const unsigned int  kBeepReadyTriTone2          = 200;
    const unsigned int  kBeepReadyTriTone3          = 100;

    const unsigned int  kBeepCollisionTriTone1      = 100;
    const unsigned int  kBeepCollisionTriTone2      = 10;
    const unsigned int  kBeepCollisionTriTone3      = 200;



    void initBeep();

    // alert() doesn't yield (it delays)
    void alert( unsigned int durationMs = kBeepDefaultBeepDuration, unsigned int tone = kBeepDefaultBeepTone );

    // beep() yields instead of delaying
    void beep( unsigned int durationMs = kBeepDefaultBeepDuration, unsigned int tone = kBeepDefaultBeepTone );

    void chirp();

    void errorChime();

    void triTone( unsigned int tone1, unsigned int tone2, unsigned int tone3 );

    inline void readyChime()   { triTone( kBeepReadyTriTone1, kBeepReadyTriTone2, kBeepReadyTriTone3 ); }

    inline void collisionChime()    { triTone( kBeepCollisionTriTone1, kBeepCollisionTriTone2, kBeepCollisionTriTone3 ); }

    void beepOn( unsigned int tone = kBeepDefaultBeepTone );

    void beepOff();

}


#endif
