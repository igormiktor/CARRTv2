/*
    DebuggingMacros.h - Macros to assist in debugging via serial connection.

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




#ifndef DebuggingMacros_h
#define DebuggingMacros_h

#if CARRT_DEBUG_SERIAL


#include "AVRTools/USART0.h"

extern Serial0 gDebugSerial;


#define DEBUG_TABLE_HEADER( S )     gDebugSerial.println(S);

#define DEBUG_TABLE_START( S )      gDebugSerial.print(millis());   \
                                    gDebugSerial.print(", ");       \
                                    gDebugSerial.print(S);

#define DEBUG_TABLE_ITEM( X )       gDebugSerial.print(", ");       \
                                    gDebugSerial.print(X);

#define DEBUG_TABLE_ITEM_V2( V )    gDebugSerial.print(", ");       \
                                    gDebugSerial.print((V).x);      \
                                    gDebugSerial.print(", ");       \
                                    gDebugSerial.print((V).y);

#define DEBUG_TABLE_ITEM_V3( V )    gDebugSerial.print(", ");       \
                                    gDebugSerial.print((V).x);      \
                                    gDebugSerial.print(", ");       \
                                    gDebugSerial.print((V).y);      \
                                    gDebugSerial.print(", ");       \
                                    gDebugSerial.print((V).z);

#define DEBUG_TABLE_END()           gDebugSerial.print("\n");


#define DEBUG_PRINT( X )            gDebugSerial.print( X );
#define DEBUG_PRINTLN( X )          gDebugSerial.println( X );



#else


#define DEBUG_TABLE_HEADER( S )
#define DEBUG_TABLE_START( S )
#define DEBUG_TABLE_ITEM( X )
#define DEBUG_TABLE_ITEM_V2( V )
#define DEBUG_TABLE_ITEM_V3( V )
#define DEBUG_TABLE_END()
#define DEBUG_PRINT( X )
#define DEBUG_PRINTLN( X )


#endif


#endif
