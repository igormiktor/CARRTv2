/*
    LSM303DHLCTestA.cpp - Testing harness for the LSM303DHLC Accelerometer driver 

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




#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/LSM303DLHC.h"

#include "Utils/VectorUtils.h"



int main()
{
    initSystem();
    initSystemClock();
    
    I2cMaster::start();
    
    Serial0 out;
    out.start( 115200 );
    
    LSM303DLHC::init();
    
    delayMilliseconds( 1000 );
    
    out.println( "LSM303DLHC Accelerometer test..." );
    
    out.println( "Calculating at rest -- do not move CARRT" );
    
    const int nZero = 5;
    Vector3Float a0( 0.0, 0.0, 0.0 );
    for ( int i = 0; i < nZero; ++i )
    {
        a0 += LSM303DLHC::getAccelerationG();
        out.print( '.' );
        delayMilliseconds( 1000 );
    }
    a0 /= static_cast<float>( nZero );
    out.println();
    out.print( "At rest:  a.x = " );
    out.print( a0.x );
    out.print( "  a.y = " );
    out.print( a0.y );
    out.print( "  a.z = " );
    out.println( a0.z );
    out.println();  
    out.println();
    out.println( "Begin test...\n" );
        
    while ( 1 )
    {
        Vector3Float a = LSM303DLHC::getAccelerationG() - a0;
        
        out.print( "a.x = " );
        out.print( a.x );
        out.print( "  a.y = " );
        out.print( a.y );
        out.print( "  a.z = " );
        out.println( a.z );
        out.println();  
        
        for ( int i = 0; i < 5; ++i )
        {
            out.print( i + 1 );
            out.println( "..." );
            delayMilliseconds( 1000 );
        }
        out.println();

     }
}



