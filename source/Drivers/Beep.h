/*
    Beep.h - Driver for CARRT's simple audio system

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


#ifndef Beep_h
#define Beep_h

#define BEEP_DEFAULT_BEEP_TONE          40
#define BEEP_DEFAULT_BEEP_DURATION      50
#define BEEP_DEFAULT_CHIRP_TONE         90
#define BEEP_DEFAULT_CHIRP_DURATION     20


namespace Beep
{

    void initBeep();

    // alert() doesn't yield (it delays)
    void alert( unsigned int durationMs = BEEP_DEFAULT_BEEP_DURATION, unsigned int tone = BEEP_DEFAULT_BEEP_TONE );

    // beep() yields instead of delaying
    void beep( unsigned int durationMs = BEEP_DEFAULT_BEEP_DURATION, unsigned int tone = BEEP_DEFAULT_BEEP_TONE );

    void chirp();

    void errorChime();

    void beepOn( unsigned int tone = BEEP_DEFAULT_BEEP_TONE );

    void beepOff();

}


#endif