/*
    LSM303DHLCTestA.cpp - Testing harness for the LSM303DHLC Magnetometer driver 

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
    
    out.println( "LSM303DLHC Magnetometer test..." );
    
    out.println( "Calculating at rest -- do not move CARRT" );
    
    const int nZero = 5;
    Vector3Long a0t( 0.0, 0.0, 0.0 );
    Vector3Long m0t( 0.0, 0.0, 0.0 );
    for ( int i = 0; i < nZero; ++i )
    {
        a0t += LSM303DLHC::getAccelerationRaw();
        m0t += LSM303DLHC::getMagnetometerRaw();
        out.print( '.' );
        delayMilliseconds( 1000 );
    }
    a0t /= nZero;
    m0t /= nZero;
    
    Vector3Int a0;
    a0.x = a0t.x;
    a0.y = a0t.y;
    a0.z = a0t.z;
    
    Vector3Int m0;
    m0.x = m0t.x;
    m0.y = m0t.y;
    m0.z = m0t.z;
    
    float h = LSM303DLHC::calculateHeadingFromRawData( m0, a0 );
    
    out.println();
    out.print( "Heading:  " );
    out.println( h );
    
    out.println();  
    out.println();
    out.println( "Begin test...\n" );
        
    while ( 1 )
    {
        Vector3Int magRaw( LSM303DLHC::getMagnetometerRaw() );

        Vector3Int accelRaw = LSM303DLHC::getAccelerationRaw();
        
        magRaw += LSM303DLHC::getMagnetometerRaw();
        magRaw /= 2;

        h = LSM303DLHC::calculateHeadingFromRawData( magRaw, accelRaw );
         
        out.print( "Heading:  " );
        out.println( h );
        
        for ( int i = 0; i < 5; ++i )
        {
            out.print( i + 1 );
            out.println( "..." );
            delayMilliseconds( 1000 );
        }
        out.println();

     }
}



