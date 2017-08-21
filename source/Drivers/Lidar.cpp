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

#include "Servo.h"


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



// Add some "local" functions and constants to the namespace

namespace Lidar
{
    const int kLidarDoesntKnowDistance      = 1;

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



    int8_t mCurrentAngle;



    uint16_t convertToPulseLenFromDegreesRelative( int8_t degrees );

    uint8_t passConfigurationParameters( uint8_t maxAcqCnt, uint8_t acqMode, uint8_t detectThresholdBypass, uint8_t refAcqCnt );

    bool waitUntilLidarReadyToRead();

    int readDistanceData();

};



void Lidar::init()
{
    reset();
    setConfiguration( Lidar::kDefault );

    Servo::init();
    Servo::setPWMFreq( 60 );  // Analog servos run at ~60 Hz updates

    slew( 0 );
}




// cppcheck-suppress unusedFunction
int Lidar::getCurrentAngle()
{
    return mCurrentAngle;
}




int Lidar::slew( int angleDegrees )
{
    // Protect against over slewing of the radar
    if ( angleDegrees > 85 )
    {
        angleDegrees = 85;
    }
    if ( angleDegrees < -85 )
    {
        angleDegrees = -85;
    }

    mCurrentAngle = angleDegrees;

    uint16_t pulseLen = convertToPulseLenFromDegreesRelative( mCurrentAngle );
    Servo::setPWM( 0, pulseLen );

    return mCurrentAngle;
}




uint16_t Lidar::convertToPulseLenFromDegreesRelative( int8_t degrees )
{
/*
 *    -90 = 155
 *      0 = 381
 *     90 = 605
 */

    int16_t tmp = 5 * static_cast<int16_t>( degrees );
    tmp /= 2;
    tmp += 381;
    return static_cast<uint16_t>( tmp );
}




bool Lidar::waitUntilLidarReadyToRead()
{
    const unsigned int kHowManyMillisecondsToWait = 25;

    const unsigned long kStopTime = millis() + kHowManyMillisecondsToWait;

    bool lidarBusy = true;

    int err = 0;
    do
    {
        uint8_t lidarStatus;
        err = I2cMaster::readSync( kLidarI2cAddress, kStatus, 1, &lidarStatus );
        lidarBusy = lidarStatus & 0x01;
    }
    while ( !err && lidarBusy && ( millis() < kStopTime ) );

    return ( lidarBusy ? false : true );
}




int Lidar::readDistanceData()
{
    uint8_t rawDistance[2];

    int err = I2cMaster::readSync( kLidarI2cAddress, kDistanceMeasuredWord, 2, rawDistance );

    if ( !err )
    {
        return static_cast<int>( ( static_cast<uint16_t>(rawDistance[0]) << 8 ) | rawDistance[1] );
    }

    return 1;
}




int Lidar::getDistanceInCm( bool useBiasCorrection )
{
    int err = I2cMaster::writeSync( kLidarI2cAddress, kDeviceCommand,
                    useBiasCorrection ? kMeasureWithBiasCorrectionCmd : kMeasureWithoutBiasCorrectionCmd );

    if ( !err && waitUntilLidarReadyToRead() )
    {
        // Lidar ready to read a value
        int distance = readDistanceData();

        // Lidar returns 1 if it can't figure out a distance
        if ( distance != kLidarDoesntKnowDistance )
        {
            return distance;
        }
    }

    return kNoValidDistance;
}




uint8_t Lidar::reset()
{
    // Lidar takes approximately 22ms to reset
    return I2cMaster::writeSync( kLidarI2cAddress, kDeviceCommand, kResetCmd );
}




uint8_t Lidar::setConfiguration( Configuration config )
{
    uint8_t err = 0;
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






uint8_t Lidar::passConfigurationParameters( uint8_t maxAcqCnt, uint8_t acqMode, uint8_t detectThresholdBypass, uint8_t refAcqCnt )
{
    uint8_t err = I2cMaster::writeSync( kLidarI2cAddress, kMaxAcquisitionCount, maxAcqCnt );
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
