/*
    Lidar.cpp - Functions for controlling CARRT's Garmin Lidar Lite v3 range sensor

    Copyright (c) 2017 Igor Mikolic-Torreira.  All right reserved.

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





#include "Lidar.h"

#include "AVRTools/GpioPinMacros.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"

#include "CarrtPins.h"

#include "TempSensor.h"

#if !defined(CARRT_DISABLE_LIDAR_SERVO) || CARRT_DISABLE_LIDAR_SERVO == 0
#include "Servo.h"
#else
#warning CARRT_DISABLE_LIDAR_SERVO defined and non-zero: Servo functionality disabled in Lidar driver
#endif


/*
 * Designed as a namespace instead of a class because there is only one
 * radar and need to access the radar from various places in CARRT.
 *
 * A class requires a single global instance; this way we can access
 * strictly through the function interface without an instances
 *
 * Note: this means this driver code is only good for ONE radar.
 * Use of multiple radars would require conversion to a class
 */




/*
 * WARNING: the Garman Lidar Lite v3 does NOT understand I2C "restart".
 * So register reads have to be sent as two separate I2C messages: first
 * a "write" message sending the register to be read; followed by a second
 * "read" message asking for data.
 *
 * The advanced form "read" message that transmits the register to be read
 * and then reads the data fails with the lidar because it signals an I2C "restart"
 * to switch from transmitting the register to receiving the data.  The lidar
 * responds to the "restart" condition by becoming confused, without explicitly
 * signaling an error.  Data read from the lidar from that point on is garbage.
 */




// Add some "local" functions and constants to the namespace

namespace Lidar
{
    const int kLidarDoesntKnowDistance      = 1;

    const uint8_t kLidarI2cAddress          = 0x62;

    const int kLidarWaitTimedOutErr         = 666;


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




    int passConfigurationParameters( uint8_t maxAcqCnt, uint8_t acqMode, uint8_t detectThresholdBypass, uint8_t refAcqCnt );

    int waitUntilLidarReadyToRead();

    int readDistanceData( int* distInCm );

};



void Lidar::init()
{
    reset();
    setConfiguration( Lidar::kDefault );

#if !defined(CARRT_DISABLE_LIDAR_SERVO) || CARRT_DISABLE_LIDAR_SERVO == 0
    Servo::init();
    Servo::setPWMFreq( 60 );  // Analog servos run at ~60 Hz updates

    slew( 0 );
#endif
}




// cppcheck-suppress unusedFunction
int Lidar::getCurrentAngle()
{
#if !defined(CARRT_DISABLE_LIDAR_SERVO) || CARRT_DISABLE_LIDAR_SERVO == 0
    return Servo::getCurrentAngle();
#else
    return 0;
#endif
}




int Lidar::slew( int angleDegrees )
{
#if !defined(CARRT_DISABLE_LIDAR_SERVO) || CARRT_DISABLE_LIDAR_SERVO == 0
    return Servo::slew( angleDegrees );
#else
    return 0;
#endif
}




int Lidar::waitUntilLidarReadyToRead()
{
    const uint8_t kMaxWaitLoopCount = 100;

    bool lidarBusy = true;
    int err = 0;

    for ( uint8_t i = 0; lidarBusy && i < kMaxWaitLoopCount; ++i )
    {
        uint8_t lidarStatus;
        err = I2cMaster::writeSync( kLidarI2cAddress, kStatus );
        if ( err )
        {
            return err;
        }
        else
        {
            err = I2cMaster::readSync( kLidarI2cAddress, 1, &lidarStatus );
            if ( err )
            {
                return err;
            }
            else
            {
               lidarBusy = lidarStatus & 0x01;
            }
        }
    }

    return ( lidarBusy ? kLidarWaitTimedOutErr : 0 );
}




int Lidar::readDistanceData( int* distInCm )
{
    int err = I2cMaster::writeSync( kLidarI2cAddress, kDistanceMeasuredWord );
    if ( !err )
    {
        uint8_t rawDistance[2];

        err = I2cMaster::readSync( kLidarI2cAddress, 2, rawDistance );
        if ( !err )
        {
            *distInCm = static_cast<int>( ( static_cast<uint16_t>(rawDistance[0]) << 8 ) | rawDistance[1] );
        }
    }
    return err;
}




int Lidar::getDistanceInCm( int* distInCm, bool useBiasCorrection )
{
    int err = I2cMaster::writeSync( kLidarI2cAddress, kDeviceCommand,
                    useBiasCorrection ? kMeasureWithBiasCorrectionCmd : kMeasureWithoutBiasCorrectionCmd );

    if ( !err )
    {
        err = waitUntilLidarReadyToRead();
        if ( !err )
        {
            // Lidar ready to read a value
            err = readDistanceData( distInCm );

            // Lidar returns 1 if it can't figure out a distance
            if ( *distInCm == kLidarDoesntKnowDistance )
            {
                return err = kNoValidDistance;
            }
        }
    }

    return err;
}




int Lidar::reset()
{
    // Lidar takes approximately 22ms to reset
    int err= I2cMaster::writeSync( kLidarI2cAddress, kDeviceCommand, kResetCmd );
    delayMilliseconds( 23 );
    return err;
}




int Lidar::setConfiguration( Configuration config )
{
    int err = 0;
    switch ( config )
    {
        default:
        case kDefault:
            err = passConfigurationParameters( 0x80,  0x08, 0x00, 0x05 );
            break;

        case kShortRangeAndHighSpeed:
            err = passConfigurationParameters( 0x1d,  0x08, 0x00, 0x05 );
            break;

        case kShortRangeAndHighestSpeed:
            err = passConfigurationParameters( 0x0d,  0x04, 0x00, 0x03 );
            break;

        case kDefaultRangeAndHigherSpeedAtShortRange:
            err = passConfigurationParameters( 0x80,  0x00, 0x00, 0x05 );
            break;

        case kMaximumRange:
            err = passConfigurationParameters( 0xff,  0x08, 0x00, 0x05 );
            break;

        case kHighSensitivityButHigherError:
            err = passConfigurationParameters( 0x80,  0x08, 0x80, 0x05 );
            break;

        case kLowSensitivityButLowerError:
            err = passConfigurationParameters( 0x80,  0x08, 0xb0, 0x05 );
            break;
    }

    return err;
}






int Lidar::passConfigurationParameters( uint8_t maxAcqCnt, uint8_t acqMode, uint8_t detectThresholdBypass, uint8_t refAcqCnt )
{
    int err = I2cMaster::writeSync( kLidarI2cAddress, kMaxAcquisitionCount, maxAcqCnt );
    if ( !err )
    {
        err = I2cMaster::writeSync( kLidarI2cAddress, kAcquisitionMode, acqMode );
        if ( !err )
        {
            err = I2cMaster::writeSync( kLidarI2cAddress, kPeakDetectThresholdBypass, detectThresholdBypass );
            if ( !err )
            {
                err = I2cMaster::writeSync( kLidarI2cAddress, kReferenceAcquisitionCount, refAcqCnt );
            }
        }
    }

    return err;
}
