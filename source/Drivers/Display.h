/*
    Display.h - Driver for CARRT's LCD

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


#ifndef Display_h
#define Display_h


#include <stdint.h>
#include <stddef.h>

#include <avr/pgmspace.h>



namespace Display
{

    enum IntegerOutputBase
    {
        kBin    = 2,        // Produce a binary representation of integers (e.g., 11 is output as 0b1011)
        kOct    = 8,        // Produce an octal representation of integers (e.g, 11 is output as 013)
        kDec    = 10,       // Produce a decimal representation of integers (e.g., 11 is output as 11
        kHex    = 16        // Produce a hexadecimal representation of integers (e.g., 11 is output as 0x0b)
    };



    /*
     * These constants are used to set the backlight color on the LCD.
     */
    enum
    {
        kBacklight_Red      = 0x1,          // Backlight red
        kBacklight_Yellow   = 0x3,          // Backlight yellow
        kBacklight_Green    = 0x2,          // Backlight green
        kBacklight_Teal     = 0x6,          // Backlight teal
        kBacklight_Blue     = 0x4,          // Backlight blue
        kBacklight_Violet   = 0x5,          // Backlight violet
        kBacklight_White    = 0x7           // Backlight white
    };


    int init();

    void clear();
    void home();

    void displayTopRow( const char* str );
    void displayBottomRow( const char* str );

    void displayTopRowP16( PGM_P str );
    void displayBottomRowP16( PGM_P str );

    void clearTopRow();
    void clearBottomRow();

    void displayOff();
    void displayOn();

    void blinkOff();
    void blinkOn();

    void cursorOff();
    void cursorOn();

    void scrollDisplayLeft();
    void scrollDisplayRight();

    void autoscrollOn();
    void autoscrollOff();

    void setCursor( uint8_t row, uint8_t col );
    int setBacklight( uint8_t color );

    void flush();

    size_t print( const char* str, bool addLn = false );
    size_t print( const uint8_t* buf, size_t size, bool addLn = false );
    size_t print( char c, bool addLn = false );
    size_t print( long n, int base = kDec, bool addLn = false );
    size_t print( unsigned long n, int base = kDec, bool addLn = false );
    size_t print( double d, int digits = 2, bool addLn = false );
    inline size_t print( int n, int base = kDec, bool addLn = false )              { return print( static_cast<long>( n ), base, addLn ); }
    inline size_t print( unsigned int n, int base = kDec, bool addLn = false )     { return print( static_cast<unsigned long>( n ), base, addLn ); }

    size_t printP16( PGM_P str, bool addLn = false );
    inline size_t printlnP16( PGM_P str )                                          { return printP16( str, true ); }

    inline size_t println( const char* str )                                       { return print( str, true ); }
    inline size_t println( const uint8_t* buf, size_t size )                       { return print( buf, size, true ); }
    inline size_t println( char c )                                                { return print( c, true ); }
    inline size_t println( unsigned char n, int base = kDec )                      { return print( n, base, true ); }
    inline size_t println( long n, int base = kDec )                               { return print( n, base, true ); }
    inline size_t println( unsigned long n, int base = kDec )                      { return print( n, base, true ); }
    inline size_t println( double d, int digits = 2 )                              { return print( d, digits, true ); }
    inline size_t println( int n, int base = kDec )                                { return print( static_cast<long>( n ), base, true ); }
    inline size_t println( unsigned int n, int base = kDec )                       { return print( static_cast<unsigned long>( n ), base, true ); }

    size_t println();

};


#endif
