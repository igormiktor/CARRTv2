/*************************************************
 *
 * Navigator -- an inertial navigation class
 *
 *************************************************/


#include "Navigator.h"

#include <math.h>

#include "AVRTools/SystemClock.h"
#include "RoverGlobals.h"
#include "VectorUtils.h"
#include "LSM303DLHC.h"
#include "L3GD20.h"



#if DEBUG_NAVIGATOR
    #define DEBUG_MACROS_ENABLE 1
#endif

#include "DebuggingMacros.h"

// extern LSM303DLHC      gLsm303;
// extern L3GD20          gL3gd20;


const float         kRadiansToDegrees       = 180.0 / 3.14159265;
const float         kDegreesToRadians       = 3.14159265 / 180.0;

const float         kIntegrationTimeStep    = 0.125;                // 1/8 sec





Navigator::Navigator() :
    mAccelerationZero( 0, 0, 0 ),
    mGyroZero( 0, 0, 0 ),
    mCurrentAcceleration( 0, 0 ),
    mCurrentSpeed( 0, 0 ),
    mCurrentPosition( 0, 0 ),
    mCurrentHeading( 0 ),
    mAccumulatedCompassDrift( 0 ),
    mAccumulationCount( 0 ),
    mMoving( kStopped )
{
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

    const int delay1 = kSamplesPerBlock * 1000 / gLsm303.accelerometerUpdateRate();
    const int delay2 = kSamplesPerBlock * 1000 / gL3gd20.gyroscopeUpdateRate();
    const int neededDelay = ( delay1 > delay2 ) ? delay1 : delay2;

    // Accumulate kNbrBlocks blocks for samples
    for ( int i = 0; i < kNbrBlocks; ++i )
    {
        // Get blocks of accelerometer and gyroscope readings
        gLsm303.getAccelerationDataBlockSync( &accelData, kSamplesPerBlock );
        gL3gd20.getAngularRatesDataBlockSync( &gyroData, kSamplesPerBlock );
        for ( int j = 0; j < kSamplesPerBlock; ++j )
        {
            a0 += gLsm303.convertDataBlockEntryToAccelerationRaw( accelData, j );
            g0 += gL3gd20.convertDataBlockEntryToAngularRatesRaw( gyroData, j );
        }

        // Get a single magnetometer reading (no FIFO buffer)
        m0 += gLsm303.getMagnetometerRaw();

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
    mCurrentHeading = gLsm303.calculateHeadingFromRawData( m, mAccelerationZero );

    DEBUG_TABLE_HEADER( "time, label, ax, ay, vx, vy, sx, sy, hdg, acd, ncd, chdg, del-c, del-g, del-gr" )
}


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
        mTmp += gLsm303.getMagnetometerRaw();
    }
    mTmp /= 16;
    Vector3Int m( mTmp.x, mTmp.y, mTmp.z );

    // Current heading estimate
    mCurrentHeading = gLsm303.calculateHeadingFromRawData( m, mAccelerationZero );
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
    // This function executes in 6.96 ms

    if ( mMoving )
    {
        // Get a "before" mag measurement
        Vector3Int magRaw( gLsm303.getMagnetometerRaw() );

        // Get the raw accelerometer data
        Vector3Int accelRaw = gLsm303.getAccelerationRaw();

        // Get the raw gyroscope data
        Vector3Int gyroRaw = gL3gd20.getAngularRatesRaw();

        // Get second "after" mag measurement and average
        magRaw += gLsm303.getMagnetometerRaw();
        magRaw /= 2;

        // Get both compass and gyro heading change estimates
        float compassHeading = gLsm303.calculateHeadingFromRawData( magRaw, accelRaw );
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

    if ( kLowerLimitX < zeroedAcceleration.x && zeroedAcceleration.x < kUpperLimitX )
    {
        zeroedAcceleration.x = 0;
    }

    if ( kLowerLimitY < zeroedAcceleration.y && zeroedAcceleration.y < kUpperLimitY )
    {
        zeroedAcceleration.y = 0;
    }

    // Step 3: Convert to units we actually can work with
    return gLsm303.convertRawToXYMetersPerSec2( zeroedAcceleration );
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
    return gL3gd20.convertRawToDegreesPerSecond( zeroedGyroZ );
}



void Navigator::updateIntegration( const Vector2Float& newAccel, Vector2Float* newSpeed, Vector2Float* newPosition )
{
    // First integration to get Speed (operator overloading means this does both axes) -- samples every 1/8 seconds

    *newSpeed = ( mCurrentAcceleration + ( newAccel - mCurrentAcceleration ) / 2 ) * kIntegrationTimeStep;
    *newSpeed += mCurrentSpeed;

    // Limit the maximum speed (prevents run-away integration)

    newSpeed->x = limitSpeed( newSpeed->x );
    newSpeed->y = limitSpeed( newSpeed->y );

    // Second integration to get Position (operator overloading means this does both axes) -- samples every 1/8 seconds

    *newPosition = ( mCurrentSpeed + ( *newSpeed - mCurrentSpeed ) / 2 ) * kIntegrationTimeStep;
    *newPosition += mCurrentPosition;
}



float Navigator::limitSpeed( float v )
{
    // Top speed 2.2m per 10s = .22m
    const float kMaxSpeed = 0.3;        // m/s
    if ( v < -kMaxSpeed )
    {
        v = -kMaxSpeed;
    }
    else if ( v > kMaxSpeed )
    {
        v = kMaxSpeed;
    }

    return v;
}





