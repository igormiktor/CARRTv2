/*
    Navigator.cpp - An Dead-Reckoning Navigation module for CARRT.
    It actually combines accelerometer, compass and gyroscope data to
    maintain orientation and uses dead-reckoning for distance traveled.

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





#include "Navigator.h"

#include <math.h>

#include "AVRTools/SystemClock.h"
#include "Utils/VectorUtils.h"
#include "Drivers/LSM303DLHC.h"
#include "Drivers/L3GD20.h"



#include "Utils/DebuggingMacros.h"







// Extend the namespace with functions and variables used internally in this module

namespace Navigator
{

    const float kRadiansToDegrees       = 180.0 / 3.14159265;
    const float kDegreesToRadians       = 3.14159265 / 180.0;

    const float kIntegrationTimeStep    = 0.125;                // 1/8 sec



    enum Motion { kStopped = 0, kStraightMove = 0x01, kTurnMove = 0x10, kComboMove = 0x11 };

    void moving( Motion kindOfMove );

    void updateOrientation( Vector3Float g, Vector3Float a, Vector3Float m );
    void updateIntegration( const Vector2Float& newAcceleration, Vector2Float* newSpeed, Vector2Float* newPosition );

    void limitSpeed( Vector2Float* v );
    float limitRotationRate( float r );

    Vector2Float filterAndConvertAccelerationDataToMetersPerSec2( const Vector3Int& in );
    float filterAndConvertGyroscopeDataToZDegreesPerSec( const Vector3Int& in );
    void determineNewHeading( float magHeadingChange, float gyroHeadingChange );

    int roundToInt( float x );



    Vector3Int      mAccelerationZero;
    Vector3Int      mGyroZero;

    Vector2Float    mCurrentAcceleration;
    Vector2Float    mCurrentSpeed;
    Vector2Float    mCurrentPosition;

    float           mCurrentHeading;
    float           mAccumulatedCompassDrift;
    int             mAccumulationCount;

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
Vector2Float Navigator::getCurrentSpeed()
{
    return mCurrentSpeed;
}


// cppcheck-suppress unusedFunction
Vector2Float Navigator::getCurrentAcceleration()
{
    return mCurrentAcceleration;
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


void Navigator::moving()
{
    moving( kComboMove );
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
Vector2Float Navigator::convertRelativeToAbsoluteCoords( int downRange, int crossRange )
{
    // First get heading in radians
    float hdg = (360 - mCurrentHeading) * kDegreesToRadians;

    float angle = atan2( crossRange, downRange ) + hdg;
    float range = sqrt( square(downRange) + square(crossRange) );

    return Vector2Float( range*cos(angle), range*sin(angle) );
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

    DEBUG_TABLE_HEADER( "time, label, ax, ay, vx, vy, sx, sy, hdg, acd, ncd, chdg, del-c, del-g, del-gr" )
}


// cppcheck-suppress unusedFunction
void Navigator::hardReset()
{
    init();
}


void Navigator::reset()
{
    mCurrentAcceleration.x = 0;
    mCurrentAcceleration.y = 0;
    mCurrentSpeed.x = 0;
    mCurrentSpeed.y = 0;
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
    mAccumulatedCompassDrift = 0;
    mAccumulationCount = 0;

    mMoving = kStopped;
}


void Navigator::moving(  Motion kindOfMove )
{
    mMoving = kindOfMove;
    mAccumulatedCompassDrift = 0;
    mAccumulationCount = 0;
}


void Navigator::stopped()
{
    mMoving = kStopped;

    mCurrentAcceleration.x = 0;
    mCurrentAcceleration.y = 0;
    mCurrentSpeed.x = 0;
    mCurrentSpeed.y = 0;
}



// cppcheck-suppress unusedFunction
void Navigator::doDriftCorrection()
{
    if ( mMoving == kStraightMove )
    {
        DEBUG_TABLE_START( "doDriftCorr before" )
        DEBUG_TABLE_ITEM_V2( mCurrentAcceleration )
        DEBUG_TABLE_ITEM_V2( mCurrentSpeed )
        DEBUG_TABLE_ITEM_V2( mCurrentPosition )
        DEBUG_TABLE_ITEM( mCurrentHeading )
        DEBUG_TABLE_ITEM( mAccumulatedCompassDrift )
        DEBUG_TABLE_ITEM( mAccumulationCount )
        DEBUG_TABLE_ITEM( ' ' )
        DEBUG_TABLE_ITEM( ' ' )
        DEBUG_TABLE_END()

        // Average out the drift, add it in, and reset
        mCurrentHeading += mAccumulatedCompassDrift / mAccumulationCount;

        // Reset
        mAccumulatedCompassDrift = 0;
        mAccumulationCount = 0;

        DEBUG_TABLE_START( "doDriftCorr after" )
        DEBUG_TABLE_ITEM_V2( mCurrentAcceleration )
        DEBUG_TABLE_ITEM_V2( mCurrentSpeed )
        DEBUG_TABLE_ITEM_V2( mCurrentPosition )
        DEBUG_TABLE_ITEM( mCurrentHeading )
        DEBUG_TABLE_ITEM( mAccumulatedCompassDrift )
        DEBUG_TABLE_ITEM( mAccumulationCount )
        DEBUG_TABLE_ITEM( ' ' )
        DEBUG_TABLE_ITEM( ' ' )
        DEBUG_TABLE_END()
    }
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

        // How far

        // Compute the current N and W vectors based on heading
        float cosHeading = cos( mCurrentHeading * kDegreesToRadians );
        float sinHeading = sin( mCurrentHeading * kDegreesToRadians );
        Vector2Float north( cosHeading, sinHeading );
        Vector2Float west( -sinHeading, cosHeading );

        // Now that we have N & W unit vectors; go ahead and massage acceleration:
        // zero (center) and filter acceleration, and turn it into units we understand
        Vector2Float aFilteredXYMetric = filterAndConvertAccelerationDataToMetersPerSec2( accelRaw );

        Vector2Float accelerationNandW( aFilteredXYMetric * north, aFilteredXYMetric * west );

        Vector2Float newSpeed, newPosition;
        updateIntegration( accelerationNandW, &newSpeed, &newPosition );

        // Update current information
        mCurrentAcceleration    = accelerationNandW;
        mCurrentSpeed           = newSpeed;
        mCurrentPosition        = newPosition;

        DEBUG_TABLE_START( "doNavUpdate" )
        DEBUG_TABLE_ITEM_V2( mCurrentAcceleration )
        DEBUG_TABLE_ITEM_V2( mCurrentSpeed )
        DEBUG_TABLE_ITEM_V2( mCurrentPosition )
        DEBUG_TABLE_ITEM( mCurrentHeading )
        DEBUG_TABLE_ITEM( mAccumulatedCompassDrift )
        DEBUG_TABLE_ITEM( mAccumulationCount )
        DEBUG_TABLE_ITEM( compassHeading )
        DEBUG_TABLE_ITEM( compassHeadingChange )
        DEBUG_TABLE_ITEM( gyroHeadingChange )
        DEBUG_TABLE_ITEM( gyroRaw.z - mGyroZero.z )
        DEBUG_TABLE_END()
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
    mAccumulatedCompassDrift += compassHeadingChange - headingChange;
    ++mAccumulationCount;

    // Limit the max turn value value
    // headingChange = limitRotationRate( headingChange );

    // Rounding (takes care of small, close to zero, noise)
    // headingChange = floor( headingChange + 0.5 );

    // Update heading
    mCurrentHeading += headingChange;

    // Correct for drift every once in a while
    if ( mAccumulationCount > 15 )
    {
        // doDriftCorrection();
        mAccumulationCount = 0;
    }

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



Vector2Float Navigator::filterAndConvertAccelerationDataToMetersPerSec2( const Vector3Int& in )
{
    // Step 1: "zero" it out -- subtract off rest-state acceleration (= gravity)
    Vector3Int zeroedAcceleration = in - mAccelerationZero;

    // Step 2: Low-pass filter to ignore small noise and not treat it as acceleration
    // Only care about x and y...
    const int kUpperLimitX = 15;
    const int kLowerLimitX = -20;
    const int kUpperLimitY = 15;
    const int kLowerLimitY = -20;

#if 1
    if ( kLowerLimitX < zeroedAcceleration.x && zeroedAcceleration.x < kUpperLimitX )
    {
        zeroedAcceleration.x = 0;
    }

    if ( kLowerLimitY < zeroedAcceleration.y && zeroedAcceleration.y < kUpperLimitY )
    {
        zeroedAcceleration.y = 0;
    }
#endif

    // Step 3: Convert to units we actually can work with
    return LSM303DLHC::convertRawToXYMetersPerSec2( zeroedAcceleration );
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



void Navigator::updateIntegration( const Vector2Float& newAccel, Vector2Float* newSpeed, Vector2Float* newPosition )
{
    // First integration to get Speed (operator overloading means this does both axes) -- samples every 1/8 seconds

    *newSpeed = ( mCurrentAcceleration + ( newAccel - mCurrentAcceleration ) / 2 ) * kIntegrationTimeStep;
    *newSpeed += mCurrentSpeed;

    // Limit the maximum speed (prevents run-away integration)
#if 1
    limitSpeed( newSpeed );
#endif

    // Second integration to get Position (operator overloading means this does both axes) -- samples every 1/8 seconds

    *newPosition = ( mCurrentSpeed + ( *newSpeed - mCurrentSpeed ) / 2 ) * kIntegrationTimeStep;
    *newPosition += mCurrentPosition;
}



void Navigator::limitSpeed( Vector2Float* v )
{
    // Top speed ~ 40 cm/s
    const float kMaxSpeed = 0.40;        // m/s

    float norm_v = norm( *v );
    if ( norm_v > kMaxSpeed )
    {
        *v *= (kMaxSpeed/norm_v);
    }
}





