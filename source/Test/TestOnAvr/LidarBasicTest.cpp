/*
    LidarBasicTest.cpp - Basic testing of how the Garman Lidar Lite v3 works.

    Copyright (c) 2018 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; withlaptop even the implied warranty of
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
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"




const uint8_t   kLidarI2cAddress       = 0x62;


enum LidarRegisters
{
    kDeviceCommand                      = 0x00,
    kStatus                             = 0x01,
    kMaxAcquisitionCount                = 0x02,             // Default:  0x80
    kAcquisitionMode                    = 0x04,             // Default:  0x08
    kVelocity                           = 0x09,
    kPeakCorrelation                    = 0x0c,
    kCorrRecordNoiseFloor               = 0x0d,
    kRecvSignalStrength                 = 0x0e,
    kDistanceMeasuredHighByte           = 0x0f,
    kDistanceMeasuredLowByte            = 0x10,
    kDistanceMeasuredWord               = 0x8f,
    kBurstMeasurmentCountControl        = 0x11,             // Default:  0x01
    kReferenceAcquisitionCount          = 0x12,             // Default:  0x05
    kPreviousDistanceMeasuredHigh       = 0x14,
    kPreviousDistanceMeasuredLow        = 0x15,
    kPreviousDistanceMeasuredWord       = 0x94,
    kUnitSerialNumberHighByte           = 0x16,
    kUnitSerialNumberLowByte            = 0x17,
    kUnitSerialNumberWord               = 0x96,
    kUnitSerialNumberWriteHighByte      = 0x18,
    kUnitSerialNumberWriteLowByte       = 0x19,
    kUnitSerialNumberWriteWord          = 0x98,
    kI2cAddressReadSet                  = 0x1a,
    kPeakDetectThresholdBypass          = 0x1c,             // Default:  0x00
    kDefaultI2cResponseControl          = 0x1e,             // Default:  0x00
    kStateCommand                       = 0x40,
    kAutoMeasurementDelay               = 0x45,             // Default:  0x14
    kSecondLargestCorrelationPeak       = 0x4c,
    kCorrelationData                    = 0x52,
    kCorrelationSign                    = 0x53,
    kCorrelationDataWordReversed        = 0xd2,
    kCorrelRecordMemBankSelect          = 0x5d,
    kPowerStateControl                  = 0x65              // Default:  0x80
};


enum LidarCommands
{
    kResetCmd                           = 0x00,
    kMeasureWithoutBiasCorrectionCmd    = 0x03,
    kMeasureWithBiasCorrectionCmd       = 0x04
};




Serial0 laptop;



int doLidarInit();
int doLidarReadVal1Call();
int doLidarReadVal2Call();
int doLidarConfig();
int doLidarPing();
int doLidarWait1Call();
int doLidarWait2Call();
int doLidarReadRange1Call( int* dist );
int doLidarReadRange2Call( int* dist );


int main()
{
    initSystem();
    initSystemClock();

    laptop.start( 115200 );

    delayMilliseconds( 3000 );

    I2cMaster::start();

    delayMilliseconds( 3000 );

    uint8_t a = 63;
    laptop.println( a, Serial0::kHex );

    int8_t b = -125;
    laptop.println( b, Serial0::kHex );


    uint16_t a2 = 511;
    laptop.println( a2, Serial0::kHex );

    int16_t b2 = -121;
    laptop.println( b2, Serial0::kHex );

    doLidarInit();

//    doLidarReadVal1Call();

    doLidarReadVal2Call();

//    doLidarReadVal1Call();

//    doLidarReadVal2Call();

    doLidarConfig();

//    doLidarReadVal1Call();

    doLidarReadVal2Call();


    while ( 1 )
    {
        delay( 3000 );

        doLidarPing();

        doLidarWait2Call();

        int dist;
        doLidarReadRange2Call( &dist );
    }

}



int doLidarInit()
{
    // Lidar takes approximately 22ms to reset
    int err = I2cMaster::writeSync( kLidarI2cAddress, kDeviceCommand, kResetCmd );
    if ( err )
    {
        laptop.print( "Lidar init() failed, error code: " );
        laptop.println( err );
    }
    else
    {
        laptop.println( "Lidar init() successful" );
        delayMilliseconds( 24 );
    }

    return err;
}


int doLidarReadVal1Call()
{
    uint8_t value = 0x66;
    int err = I2cMaster::readSync( kLidarI2cAddress, 0x02, 1, &value );

    if ( err )
    {
        laptop.print( "Lidar readVal1Call() failed, error code: " );
        laptop.println( err );
    }
    else
    {
        laptop.print( "Lidar readVal1Call() read value " );
        laptop.println( value, Writer::kHex );
    }

    delay( 1 );
    return err;
}



int doLidarReadVal2Call()
{
    uint8_t value = 0x33;
    int err = I2cMaster::writeSync( kLidarI2cAddress, 0x02 );

    if ( err )
    {
        laptop.print( "Lidar readVal2Calls() write failed, error code: " );
        laptop.println( err );
    }
    else
    {
        delay( 1 );
        err = I2cMaster::readSync( kLidarI2cAddress, 1, &value );
        if ( err )
        {
            laptop.print( "Lidar readVal2Calls() read failed, error code: " );
            laptop.println( err );
        }
        else
        {
            laptop.print( "Lidar readVal2Calls() read value " );
            laptop.println( value, Writer::kHex );
        }
    }

    return err;
}



int doLidarConfig()
{
    int err = I2cMaster::writeSync( kLidarI2cAddress, (uint8_t) 0x02, (uint8_t) 0x80 );
    if ( err )
    {
        laptop.print( "Lidar config() failed step 0x02, error code: " );
        laptop.println( err );
    }
    else
    {
        err = I2cMaster::writeSync( kLidarI2cAddress, (uint8_t) 0x04, (uint8_t) 0x08 );
        if ( err )
        {
            laptop.print( "Lidar config() failed step 0x04, error code: " );
            laptop.println( err );
        }
        else
        {
            err = I2cMaster::writeSync( kLidarI2cAddress, (uint8_t) 0x1c, (uint8_t) 0x00 );
            if ( err )
            {
                laptop.print( "Lidar config() failed step 0x1c, error code: " );
                laptop.println( err );
            }
            else
            {
                laptop.println( "Lidar config() successful" );
            }
        }
    }

    return err;
}



int doLidarPing()
{
    int err = I2cMaster::writeSync( kLidarI2cAddress, (uint8_t) 0x00, (uint8_t) 0x04 );
    if ( err )
    {
        laptop.print( "Lidar ping() failed, error code: " );
        laptop.println( err );
    }
    return err;
}



int doLidarWait1Call()
{
    uint8_t busy = 1;

    int count = 0;
    int err = 0;

    while ( busy )
    {
        uint8_t lidarStatus = 0;
        err = I2cMaster::readSync( kLidarI2cAddress, 0x01, 1, &lidarStatus );
        if ( err )
        {
            laptop.print( "Lidar Wait1Call() read failed, error code: " );
            laptop.println( err );
            break;
        }

        busy = lidarStatus & 0x01;
        ++count;

        if ( count > 9999 )
        {
            laptop.println( "Lidar Wait1Call timed out" );
            break;
        }
    }

    if ( !busy )
    {
        // We got here the normal way
        laptop.print( "Lidar Wait1Call() was busy for a count of " );
        laptop.println( count );
    }

    return err;
}



int doLidarWait2Call()
{
    uint8_t busy = 1;

    int count = 0;
    int err = 0;

    while ( busy )
    {
        uint8_t lidarStatus = 0;
        err = I2cMaster::writeSync( kLidarI2cAddress, 0x01 );
        if ( err )
        {
            laptop.print( "Lidar Wait2Call() read failed, error code: " );
            laptop.println( err );
            break;
        }
        else
        {
            err = I2cMaster::readSync( kLidarI2cAddress, 1, &lidarStatus );
            if ( err )
            {
                laptop.print( "Lidar Wait2Call() read failed, error code: " );
                laptop.println( err );
                break;
            }
        }

        busy = lidarStatus & 0x01;
        ++count;

        if ( count > 9999 )
        {
            laptop.println( "Lidar Wait2Call() timed out" );
            break;
        }
    }

    if ( !busy )
    {
        // We got here the normal way
        laptop.print( "Lidar Wait2Call() was busy for a count of " );
        laptop.println( count );
    }

    return err;
}



int doLidarReadRange1Call( int* dist )
{
    uint8_t rawDistance[2];
    rawDistance[0] = 0;
    rawDistance[1] = 1;

    int err = I2cMaster::readSync( kLidarI2cAddress, 0x8f, 2, rawDistance );
    if ( err )
    {
        laptop.print( "Lidar ReadRange1Call() failed, error code: " );
        laptop.println( err );
    }
    else
    {
        laptop.print( "Lidar ReadRange1Call() success:  " );
        laptop.print( rawDistance[0] );
        laptop.print( ", " );
        laptop.println( rawDistance[1] );

        int d = static_cast<int>( ( static_cast<uint16_t>(rawDistance[0]) << 8 ) | rawDistance[1] );

        laptop.print( "Lidar ReadRange1Call() computed distance " );
        laptop.println( d );

        *dist = d;
    }

    return err;
}



int doLidarReadRange2Call( int* dist )
{
    uint8_t rawDistance[2];

    int err = I2cMaster::writeSync( kLidarI2cAddress, 0x8f );
    if ( err )
    {
        laptop.print( "Lidar ReadRange2Call() write failed, error code: " );
        laptop.println( err );
    }
    else
    {
        err = I2cMaster::readSync( kLidarI2cAddress, 2, rawDistance );
        if ( err )
        {
            laptop.print( "Lidar ReadRange2Call() read failed, error code: " );
            laptop.println( err );
        }
        else
        {
            laptop.print( "Lidar ReadRange2Call() success:  " );
            laptop.print( rawDistance[0] );
            laptop.print( ", " );
            laptop.println( rawDistance[1] );

            int d = static_cast<int>( ( static_cast<uint16_t>(rawDistance[0]) << 8 ) | rawDistance[1] );

            laptop.print( "Lidar ReadRange2Call() computed distance " );
            laptop.println( d );

            *dist = d;
        }
    }

    return err;
}

