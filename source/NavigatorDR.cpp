/*
    Navigator.cpp - An Dead-Reckoning Navigation module for CARRT.
    It actually combines accelerometer, compass and gyroscope data to
    maintain orientation and uses dead-reckoning for distance traveled.

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





#include "Navigator.h"

#include <math.h>

#include "AVRTools/SystemClock.h"
#include "Utils/VectorUtils.h"
#include "Drivers/DriveParam.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/L3GD20.h"



#include "Utils/DebuggingMacros.h"


#if CARRT_ENABLE_NAVIGATOR_DEBUG

#define NAV_DEBUG_TABLE_HEADER( S )     DEBUG_TABLE_HEADER( S )
#define NAV_DEBUG_TABLE_START( S )      DEBUG_TABLE_START( S )
#define NAV_DEBUG_TABLE_ITEM( X )       DEBUG_TABLE_ITEM( X )
#define NAV_DEBUG_TABLE_ITEM_V2( V )    DEBUG_TABLE_ITEM_V2( V )
#define NAV_DEBUG_TABLE_ITEM_V3( V )    DEBUG_TABLE_ITEM_V3( V )
#define NAV_DEBUG_TABLE_END()           DEBUG_TABLE_END()

#else

#define NAV_DEBUG_TABLE_HEADER( S )
#define NAV_DEBUG_TABLE_START( S )
#define NAV_DEBUG_TABLE_ITEM( X )
#define NAV_DEBUG_TABLE_ITEM_V2( V )
#define NAV_DEBUG_TABLE_ITEM_V3( V )
#define NAV_DEBUG_TABLE_END()

#endif





// Extend the namespace with functions and variables used internally in this module

namespace Navigator
{

    const float kRadiansToDegrees       = 180.0 / 3.14159265;
    const float kDegreesToRadians       = 3.14159265 / 180.0;

    const float kIntegrationTimeStep    = 0.125;                // 1/8 sec



    enum Motion { kStopped = 0, kStraightMove = 0x01, kTurnMove = 0x10 };

    void moving( Motion kindOfMove );

    void updateOrientation( Vector3Float g, Vector3Float a, Vector3Float m );

    float limitRotationRate( float r );

    float filterAndConvertGyroscopeDataToZDegreesPerSec( const Vector3Int& in );
    void determineNewHeading( float magHeadingChange, float gyroHeadingChange );

    int roundToInt( float x );


    // In DR model of motion, acceleration is always zero (treat as "instantaneous")
    // Rotation handled by compass and gryo, not accelerometer.

    Vector3Int      mAccelerationZero;
    Vector3Int      mGyroZero;

    Vector2Float    mCurrentVelocity;
    Vector2Float    mCurrentPosition;

    float           mCurrentHeading;

    Motion          mMoving;

};



int Navigator::roundToInt( float x )
{
    return static_cast<int>( x >= 0 ? x + 0.5 : x - 0.5 );
}


float Navigator::getCurrentHeading()
{
    return mCurrentHeading;
}


Vector2Float Navigator::getCurrentPosition()
{
    return mCurrentPosition;
}


Vector2Float Navigator::getCurrentPositionCm()
{
    return mCurrentPosition * 100.0;
}


// cppcheck-suppress unusedFunction
Vector2Float Navigator::getCurrentVelocity()
{
    return mCurrentVelocity;
}


// cppcheck-suppress unusedFunction
Vector2Float Navigator::getCurrentAcceleration()
{
    return Vector2Float( 0, 0 );
}


// cppcheck-suppress unusedFunction
Vector3Int Navigator::getRestStateAcceleration()
{
    return mAccelerationZero;
}


// cppcheck-suppress unusedFunction
Vector3Int Navigator::getRestStateAngularRate()
{
    return mGyroZero;
}


void Navigator::movingStraight()
{
    moving( kStraightMove );
}


void Navigator::movingTurning()
{
    moving( kTurnMove );
}


bool Navigator::isMoving()
{
    return mMoving;
}







// Forward and right are positive
Vector2Float Navigator::convertRelativeToAbsoluteCoordsCm( int downRange, int crossRange )
{
    // First get heading in radians
    float hdg = (360 - mCurrentHeading) * kDegreesToRadians;

    float angle = atan2( crossRange, downRange ) + hdg;
    float range = sqrt( square(downRange) + square(crossRange) );

    return Vector2Float( range*cos(angle), range*sin(angle) ) + getCurrentPositionCm();
}


Vector2Float Navigator::convertRelativeToAbsoluteCoordsMeter( int downRange, int crossRange )
{
    // First get heading in radians
    float hdg = (360 - mCurrentHeading) * kDegreesToRadians;

    float angle = atan2( crossRange, downRange ) + hdg;
    float range = sqrt( square(downRange) + square(crossRange) );

    return Vector2Float( range*cos(angle), range*sin(angle) ) + getCurrentPosition();
}



int Navigator::convertToCompassAngle( float mathAngle )
{
    return ( roundToInt( 360.0 - mathAngle * kRadiansToDegrees ) + 360 ) % 360;
}








void Navigator::init()
{
    // Figure out the accelerometer zero point and store in mAccelerationZero
    // Figure out the gyroscope zero point and store in mGyroZero

    const uint8_t   kSamplesPerBlock    = 32;       // Each block is 32 individual readings
    const int       kNbrBlocks          = 10;       // 10 blocks = 320 values

    DataBlock accelData;
    DataBlock gyroData;

    // Need 32 bits to store the accumulation without overflow
    Vector3Long a0( 0, 0, 0 );
    Vector3Long g0( 0, 0, 0 );
    Vector3Long m0( 0, 0, 0 );

    const int delay1 = kSamplesPerBlock * 1000 / LSM303DLHC::accelerometerUpdateRate();
    const int delay2 = kSamplesPerBlock * 1000 / L3GD20::gyroscopeUpdateRate();
    const int neededDelay = ( delay1 > delay2 ) ? delay1 : delay2;

    // Accumulate kNbrBlocks blocks for samples
    for ( int i = 0; i < kNbrBlocks; ++i )
    {
        // Get blocks of accelerometer and gyroscope readings
        LSM303DLHC::getAccelerationDataBlockSync( &accelData, kSamplesPerBlock );
        L3GD20::getAngularRatesDataBlockSync( &gyroData, kSamplesPerBlock );
        for ( int j = 0; j < kSamplesPerBlock; ++j )
        {
            a0 += LSM303DLHC::convertDataBlockEntryToAccelerationRaw( accelData, j );
            g0 += L3GD20::convertDataBlockEntryToAngularRatesRaw( gyroData, j );
        }

        // Get a single magnetometer reading (no FIFO buffer)
        m0 += LSM303DLHC::getMagnetometerRaw();

        if ( i != ( kNbrBlocks - 1 ) )
        {
            // Delay if it isn't the last time through (no need to delay the last time through)
            delay( neededDelay );
        }
    }

    // Divide by the total number of samples
    a0 /= kNbrBlocks * kSamplesPerBlock;
    g0 /= kNbrBlocks * kSamplesPerBlock;
    m0 /= kNbrBlocks;

    // Store as the zero-point of acceleration (note it is a Vector3Int)
    mAccelerationZero.x = a0.x;
    mAccelerationZero.y = a0.y;
    mAccelerationZero.z = a0.z;

    // Store as the zero-point of gyroscope (note it is a Vector3Int)
    mGyroZero.x = g0.x;
    mGyroZero.y = g0.y;
    mGyroZero.z = g0.z;

    // Start clean
    reset();

    // Convert the magnetometer readings
    Vector3Int m( m0.x, m0.y, m0.z );
    // This intentionally replaces the value set in reset()
    mCurrentHeading = LSM303DLHC::calculateHeadingFromRawData( m, mAccelerationZero );

    NAV_DEBUG_TABLE_HEADER( "time, label, ax, ay, vx, vy, sx, sy, hdg, chdg, del-c, del-g, del-gr" )
}


// cppcheck-suppress unusedFunction
void Navigator::hardReset()
{
    init();
}


void Navigator::reset()
{
    mCurrentVelocity.x = 0;
    mCurrentVelocity.y = 0;
    mCurrentPosition.x = 0;
    mCurrentPosition.y = 0;

    // Get an estimate of the heading
    Vector3Long mTmp( 0, 0, 0 );
    for ( int i = 0; i < 16; ++i )
    {
        mTmp += LSM303DLHC::getMagnetometerRaw();
    }
    mTmp /= 16;
    Vector3Int m( mTmp.x, mTmp.y, mTmp.z );

    // Current heading estimate
    mCurrentHeading = LSM303DLHC::calculateHeadingFromRawData( m, mAccelerationZero );

    mMoving = kStopped;
}


void Navigator::moving(  Motion kindOfMove )
{
    mMoving = kindOfMove;

    if ( kindOfMove == kStraightMove )
    {
        // Get the components of velocity...
        // N -> x; W -> y; compass -> radians flips direction from clockwise to counter-clockwise

        float speed = DriveParam::getFullSpeedMetersPerSec();
        mCurrentVelocity.x = speed * cos( mCurrentHeading * kDegreesToRadians );           // cos(-x) == cos(x)
        mCurrentVelocity.y = -speed * sin( mCurrentHeading * kDegreesToRadians );          // sin(-x) == -sin(x)
    }
    else
    {
        // No net velocity
        mCurrentVelocity.x = 0;
        mCurrentVelocity.y = 0;
    }
}


void Navigator::stopped()
{
    mMoving = kStopped;

    mCurrentVelocity.x = 0;
    mCurrentVelocity.y = 0;
}




void Navigator::doNavUpdate()
{
    // This function executes in 9.13 ms

    if ( mMoving )
    {
        // Get a "before" mag measurement
        Vector3Int magRaw( LSM303DLHC::getMagnetometerRaw() );

        // Get the raw accelerometer data
        Vector3Int accelRaw = LSM303DLHC::getAccelerationRaw();

        // Get the raw gyroscope data
        Vector3Int gyroRaw = L3GD20::getAngularRatesRaw();

        // Get second "after" mag measurement and average
        magRaw += LSM303DLHC::getMagnetometerRaw();
        magRaw /= 2;

        // Get both compass and gyro heading change estimates
        float compassHeading = LSM303DLHC::calculateHeadingFromRawData( magRaw, accelRaw );
        float compassHeadingChange = compassHeading - mCurrentHeading;
        // Handle the 360<->0, NE-NW crossing
        if ( compassHeadingChange > 180 )
        {
            // Going from NE to NW
            compassHeadingChange -= 360;
        }
        else if ( compassHeadingChange < -180 )
        {
            // Going from NW to NE
            compassHeadingChange += 360;
        }
        float gyroHeadingChange = -filterAndConvertGyroscopeDataToZDegreesPerSec( gyroRaw ) * kIntegrationTimeStep;

        determineNewHeading( compassHeadingChange, gyroHeadingChange );

        // How far; apply direct reconing
        if ( mMoving == kStraightMove )
        {
            mCurrentPosition += ( mCurrentVelocity / 8 );
        }
        else
        {
            // No net change in position in any other kind of kindOfMove
        }

        NAV_DEBUG_TABLE_START( "doNavUpdate" )
        NAV_DEBUG_TABLE_ITEM_V2( mCurrentVelocity )
        NAV_DEBUG_TABLE_ITEM_V2( mCurrentPosition )
        NAV_DEBUG_TABLE_ITEM( mCurrentHeading )
        NAV_DEBUG_TABLE_ITEM( compassHeading )
        NAV_DEBUG_TABLE_ITEM( compassHeadingChange )
        NAV_DEBUG_TABLE_ITEM( gyroHeadingChange )
        NAV_DEBUG_TABLE_ITEM( gyroRaw.z - mGyroZero.z )
        NAV_DEBUG_TABLE_END()
    }
}



void Navigator::determineNewHeading( float compassHeadingChange, float gyroHeadingChange )
{
    float headingChange;
//    const float kWeight = 0.8;
    switch ( mMoving )
    {
        case kStopped:
            // This shouldn't happen
            headingChange = 0;
            break;

        case kStraightMove:
#if 0
            // Simple rule:  average the two
            headingChange = ( gyroHeadingChange + compassHeadingChange ) / 2.0;
#endif
#if 0
            // Simple rule:  weighted average the two
            headingChange = kWeight * gyroHeadingChange + (1 - kWeight) * compassHeadingChange;
#endif
#if 0
            // Simple rule:  go with the smallest change
            headingChange = gyroHeadingChange;
            if ( fabs( compassHeadingChange ) < fabs( gyroHeadingChange ) )
            {
                headingChange = compassHeadingChange;
            }
#endif
#if 1
            // Averaging rule:  If compass heading isn't too big, average
            headingChange = gyroHeadingChange;
            if ( fabs( compassHeadingChange ) < 4 )
            {
                headingChange = ( compassHeadingChange + gyroHeadingChange ) / 2.0;
            }
            else
            {
                // If compass heading is too big, disregard compass
                headingChange = gyroHeadingChange;
            }
#endif
            break;

        case kTurnMove:
        default:
#if 0
            // Simple rule:  average the two
            headingChange = ( gyroHeadingChange + compassHeadingChange ) / 2.0;
#endif
#if 1
            // Simple rule:  go with the smallest change
            headingChange = gyroHeadingChange;
            if ( fabs( compassHeadingChange ) < fabs( gyroHeadingChange ) )
            {
                headingChange = compassHeadingChange;
            }
#endif
            break;
    }

    // Accumulate compass drift and count (so we an average)
#if FIX_THIS
    mAccumulatedCompassDrift += compassHeadingChange - headingChange;
    ++mAccumulationCount;
#endif

    // Limit the max turn value value
    // headingChange = limitRotationRate( headingChange );

    // Rounding (takes care of small, close to zero, noise)
    // headingChange = floor( headingChange + 0.5 );

    // Update heading
    mCurrentHeading += headingChange;

#if FIX_THIS
    // Correct for drift every once in a while
    if ( mAccumulationCount > 15 )
    {
        // doDriftCorrection();
        mAccumulationCount = 0;
    }
#endif

    if ( mCurrentHeading < 0 )
    {
        mCurrentHeading += 360;
    }
    else if ( mCurrentHeading > 360 )
    {
        mCurrentHeading -= 360;
    }
}


// cppcheck-suppress unusedFunction
float Navigator::limitRotationRate( float r )
{
    // 300 deg in 10s = 30 deg/s, provide cushion...
    const float kMaxTurn = 40 * kIntegrationTimeStep;
    if ( r < -kMaxTurn )
    {
        r = -kMaxTurn;
    }
    else if ( r > kMaxTurn )
    {
        r = kMaxTurn;
    }

    return r;
}


float Navigator::filterAndConvertGyroscopeDataToZDegreesPerSec( const Vector3Int& in )
{
    // Step 1: "zero" it out -- subtract off rest-state gyro data
    int zeroedGyroZ = in.z - mGyroZero.z;

    // Step 2: Low-pass filter to ignore small noise and not treat it as acceleration
    // Limit derived from analysis of gyro noise data
    const int kLowerLimitZ = -100;          // Negative (clockwise) rotation
    const int kUpperLimitZ =  100;          // Positive (counter-clockwise) rotation

    if ( kLowerLimitZ < zeroedGyroZ && zeroedGyroZ < kUpperLimitZ )
    {
        zeroedGyroZ = 0;
    }

    // Step 3: Convert to units we actually can work with
    return L3GD20::convertRawToDegreesPerSecond( zeroedGyroZ );
}






