/*
    DisplayAndKeypad.cpp - Driver for CARRT's LCD and Key Pad
    (Note interface is spread across two header files).

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



#include "Display.h"
#include "Keypad.h"

#include "AVRTools/I2cLcd.h"





namespace
{

    I2cLcd  sDisplay;

};





/*******************************************************
 *
 *
 *  namespace Keypad
 *
 *
 ******************************************************/


uint8_t Keypad::readButtons()
{
    return sDisplay.readButtons();
}








/*******************************************************
 *
 *
 *  namespace Display
 *
 *
 ******************************************************/


int Display::init()
{
    return sDisplay.init();
}



void Display::clear()
{
    sDisplay.clear();
}



void Display::home()
{
    sDisplay.home();
}



void Display::displayTopRow( const char* str )
{
    sDisplay.displayTopRow( str );
}



void Display::displayBottomRow( const char* str )
{
    sDisplay.displayBottomRow( str );
}



void Display::clearTopRow()
{
    sDisplay.clearTopRow();
}



void Display::clearBottomRow()
{
    sDisplay.clearBottomRow();
}



void Display::displayOff()
{
    sDisplay.displayOff();
}



void Display::displayOn()
{
    sDisplay.displayOn();
}




void Display::blinkOff()
{
    sDisplay.blinkOff();
}



void Display::blinkOn()
{
    sDisplay.blinkOn();
}




void Display::cursorOff()
{
    sDisplay.cursorOff();
}



void Display::cursorOn()
{
    sDisplay.cursorOn();
}




void Display::scrollDisplayLeft()
{
    sDisplay.scrollDisplayLeft();
}



void Display::scrollDisplayRight()
{
    sDisplay.scrollDisplayRight();
}




void Display::autoscrollOn()
{
    sDisplay.autoscrollOn();
}



void Display::autoscrollOff()
{
    sDisplay.autoscrollOff();
}




void Display::setCursor( uint8_t row, uint8_t col )
{
    sDisplay.setCursor( row, col );
}



int Display::setBacklight( uint8_t color )
{
    sDisplay.setBacklight( color );
}




void Display::flush()
{
    sDisplay.flush();
}




size_t Display::print( const char* str, bool addLn )
{
    sDisplay.print( str, addLn );
}



size_t Display::print( const uint8_t* buf, size_t size, bool addLn )
{
    sDisplay.print( buf, size, addLn );
}



size_t Display::print( char c, bool addLn )
{
    sDisplay.print( c, addLn );
}



size_t Display::print( int n, int base, bool addLn )
{
    sDisplay.print( n, base, addLn );
}



size_t Display::print( unsigned int n, int base, bool addLn )
{
    sDisplay.print( n, base, addLn );
}



size_t Display::print( long n, int base, bool addLn )
{
    sDisplay.print( n, base, addLn );
}



size_t Display::print( unsigned long n, int base, bool addLn )
{
    sDisplay.print( n, base, addLn );
}



size_t Display::print( double d, int digits, bool addLn )
{
    sDisplay.print( d, digits, addLn );
}



size_t Display::println( const char* str )
{
    sDisplay.print( str, true );
}



size_t Display::println( const uint8_t* buf, size_t size )
{
    sDisplay.print( buf, size, true );
}



size_t Display::println( char c )
{
    sDisplay.print( c, true );
}



size_t Display::println( unsigned char n, int base )
{
    sDisplay.print( n, base, true );
}



size_t Display::println( int n, int base )
{
    sDisplay.print( n, base, true );
}



size_t Display::println( unsigned int n, int base )
{
    sDisplay.print( n, base, true );
}



size_t Display::println( long n, int base )
{
    sDisplay.print( n, base, true );
}



size_t Display::println( unsigned long n, int base )
{
    sDisplay.print( n, base, true );
}



size_t Display::println( double d, int digits )
{
    sDisplay.print( d, digits, true );
}




size_t Display::println()
{
    sDisplay.println();
}



