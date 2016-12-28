/*
    MenuSramTest.cpp - Testing harness for comparing PROGMEM-base menus
    vs SRAM-based menus.  Big SRAM savings result.

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



#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/USART0.h"




struct MenuItem
{
    const char*        mMenuLabel;
    uint8_t            mMenuId;
};

typedef MenuItem   MenuList;


namespace
{

    const char gTestMenuTitle[] = "Test Menu";
    const char gTestMenuItem0[] = "ABCDEFGHIJ";
    const char gTestMenuItem1[] = "abcdefghij";
    const char gTestMenuItem2[] = "0123456789";
    const char gTestMenuItem3[] = "0123456789012345";
    const char gTestMenuItem4[] = "KLMNOPQRSTUVWXYZ";
    const char gTestMenuItem5[] = "klmnopqrstuvwxyz";
    const char gTestMenuItem6[] = "09876543210";
    const char gTestMenuItem7[] = "0123456789ABCDEF";

    const MenuList gTestMenu[] =
    {
        { gTestMenuItem0,   0 },
        { gTestMenuItem1,   1 },
        { gTestMenuItem2,   2 },
        { gTestMenuItem3,   3 },
        { gTestMenuItem4,   4 },
        { gTestMenuItem5,   5 },
        { gTestMenuItem6,   6 },
        { gTestMenuItem7,   7 }
    };

}





int main()
{
    initSystem();
    initSystemClock();

    Serial0 out;
    out.start( 115200 );

    delayMilliseconds( 1000 );

    out.println( "SRAM Menu test...\n" );

    uint8_t n = sizeof( gTestMenu ) / sizeof( MenuItem );

    while ( 1 )
    {
        out.print( "Menu title: " );
        out.println( gTestMenuTitle );
        out.print( "Menu size: " );
        out.println( n );

        for ( int i = 0; i < n; ++i )
        {
            out.print( "Menu item: " );
            out.print( gTestMenu[i].mMenuLabel );
            out.print( "  Menu ID: " );
            uint8_t byte = gTestMenu[i].mMenuId;
            out.println( byte );
        }

        out.print( "\n\n\n" );
    }
}

