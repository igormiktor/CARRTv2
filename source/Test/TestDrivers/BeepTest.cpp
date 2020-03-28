/*
    BeepTest.cpp - Testing harness for the audio driver

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




#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/USART0.h"

#include "Drivers/Beep.h"



void doInstructions();
void respondToInput();
void doTone( char* token );
void doTriTone( char* token );
void doAlert();
void doBeep();
void doChirp();
void doError();
void doReady();


Serial0 laptop;



int main()
{
    initSystem();
    initSystemClock();

    laptop.start( 115200 );

    delayMilliseconds( 1000 );

    Beep::initBeep();

    doInstructions();

    while ( 1 )
    {
        if ( laptop.available() )
        {
            respondToInput();
        }
        else
        {
            delayMilliseconds( 250 );
        }
    }
}





void doInstructions()
{
    laptop.println( "Beep test: demo various sounds..." );
    laptop.println( "Enter a (or A) for an alert sound" );
    laptop.println( "Enter b (or B) for a beep sound" );
    laptop.println( "Enter c (or C) for a chirp sound" );
    laptop.println( "Enter e (or E) for an error sound" );
    laptop.println( "Enter r (or R) for a ready sound" );
    laptop.println( "Enter t (or T) followed by nbr to sound that tone" );
    laptop.println( "Enter m (or M) followed by three nbr to sound a tri-tone" );
}



void respondToInput()
{
    char input[81];

    laptop.readLine( input, 80 );
    laptop.print( "Input:  " );
    laptop.println( input );

    char* token;
    token = strtok( input, " \t" );

    if ( token )
    {
        switch ( *token )
        {
            case 'a':
            case 'A':
                laptop.println( "Alert sound..." );
                doAlert();
                break;

            case 'b':
            case 'B':
                doBeep();
                break;

            case 'c':
            case 'C':
                doChirp();
                break;

            case 'e':
            case 'E':
                doError();
                break;

            case 't':
            case 'T':
                doTone( token );
                break;

            case 'm':
            case 'M':
                doTriTone( token );
                break;

            case 'r':
            case 'R':
                doReady();
                break;

        }
    }
}



void doTone( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        unsigned int tone = atoi( token );

        Beep::beepOn( tone );
        delayMilliseconds( 1000 );
        Beep::beepOff();
    }
    else
    {
        laptop.println( "No valid tone number" );
    }
}



void doTriTone( char* token )
{
    bool err = false;

    laptop.print( "Tri-tone: " );

    token = strtok( 0, " ,\t" );
    if ( token )
    {
        unsigned int tone1 = atoi( token );
        laptop.print( tone1 );
        laptop.print( ",  " );

        token = strtok( 0, " ,\t" );
        if ( token )
        {
            unsigned int tone2 = atoi( token );
            laptop.print( tone2 );
            laptop.print( ",  " );

            token = strtok( 0, " ,\t" );
            if ( token )
            {
                unsigned int tone3 = atoi( token );
                laptop.println( tone3 );

                Beep::triTone( tone1, tone2, tone3 );
            }
            else
            {
                err = true;
            }
        }
        else
        {
            err = true;
        }
    }
    else
    {
        err = true;
    }

    if ( err )
    {
        laptop.println( "Missing 3 valid tone numbers" );
    }
}





void doAlert()
{
    Beep::alert();
}


void doBeep()
{
    Beep::beep();
}


void doChirp()
{
    Beep::chirp();
}


void doError()
{
    Beep::errorChime();
}


void doReady()
{
    Beep::readyChime();
}
