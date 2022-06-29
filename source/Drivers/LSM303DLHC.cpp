/*
    LSM303DLHC.cpp - Driver for the LSM303DLHC Accelerometer and Magnetometer/Compass
    used on CARRT (specifically it is the Adafruit LSM303DLHC Breakout board).

    Copyright (c) 2022 Igor Mikolic-Torreira.  All right reserved.

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




/*
    Inspired by, and had its origins in, the Adafruit library for the LSM303DLHC,
    but it has been heavily modified and no longer resembles the Adafruit original
*/



#include "LSM303DLHC.h"

#include <inttypes.h>
#include <math.h>

#include "AVRTools/I2cMaster.h"

#include "Utils/VectorUtils.h"


// Data rates

#define     LSM303_ACCEL_DATA_RATE_1_Hz         0b00010111
#define     LSM303_ACCEL_DATA_RATE_10_Hz        0b00100111
#define     LSM303_ACCEL_DATA_RATE_25_Hz        0b00110111
#define     LSM303_ACCEL_DATA_RATE_50_Hz        0b01000111
#define     LSM303_ACCEL_DATA_RATE_100_Hz       0b01010111
#define     LSM303_ACCEL_DATA_RATE_200_Hz       0b01100111
#define     LSM303_ACCEL_DATA_RATE_400_Hz       0b01110111

// Data rates

#define     LSM303_MAG_DATA_RATE_0p75_Hz        0b00000000
#define     LSM303_MAG_DATA_RATE_1p5_Hz         0b00000100
#define     LSM303_MAG_DATA_RATE_3p0_Hz         0b00001000
#define     LSM303_MAG_DATA_RATE_7p5_Hz         0b00001100
#define     LSM303_MAG_DATA_RATE_15_Hz          0b00010000
#define     LSM303_MAG_DATA_RATE_30_Hz          0b00010100
#define     LSM303_MAG_DATA_RATE_75_Hz          0b00011000
#define     LSM303_MAG_DATA_RATE_220_Hz         0b00011100



namespace LSM303DLHC
{

    void getGaussPerLsb( uint16_t* xy, uint16_t* z );

    LSM303MagnetometerGain mMagnetometerGain;


    //  I2C Address (accelerometer and magnetometer have separate addresses)

    const uint8_t kLSM303_AddressAccelerometer      (0x32 >> 1);         // 0011001x
    const uint8_t kLSM303_AddessMagnetometer        (0x3C >> 1);         // 0011110x


    //  Accelerometer registers

    enum
    {                                                       // DEFAULT    TYPE
        LSM303_REGISTER_ACCEL_CTRL_REG1_A           = 0x20, // 00000111   rw
        LSM303_REGISTER_ACCEL_CTRL_REG2_A           = 0x21, // 00000000   rw
        LSM303_REGISTER_ACCEL_CTRL_REG3_A           = 0x22, // 00000000   rw
        LSM303_REGISTER_ACCEL_CTRL_REG4_A           = 0x23, // 00000000   rw
        LSM303_REGISTER_ACCEL_CTRL_REG5_A           = 0x24, // 00000000   rw
        LSM303_REGISTER_ACCEL_CTRL_REG6_A           = 0x25, // 00000000   rw
        LSM303_REGISTER_ACCEL_REFERENCE_A           = 0x26, // 00000000   r
        LSM303_REGISTER_ACCEL_STATUS_REG_A          = 0x27, // 00000000   r
        LSM303_REGISTER_ACCEL_OUT_X_L_A             = 0x28,
        LSM303_REGISTER_ACCEL_OUT_X_H_A             = 0x29,
        LSM303_REGISTER_ACCEL_OUT_Y_L_A             = 0x2A,
        LSM303_REGISTER_ACCEL_OUT_Y_H_A             = 0x2B,
        LSM303_REGISTER_ACCEL_OUT_Z_L_A             = 0x2C,
        LSM303_REGISTER_ACCEL_OUT_Z_H_A             = 0x2D,
        LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A       = 0x2E,
        LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A        = 0x2F,
        LSM303_REGISTER_ACCEL_INT1_CFG_A            = 0x30,
        LSM303_REGISTER_ACCEL_INT1_SOURCE_A         = 0x31,
        LSM303_REGISTER_ACCEL_INT1_THS_A            = 0x32,
        LSM303_REGISTER_ACCEL_INT1_DURATION_A       = 0x33,
        LSM303_REGISTER_ACCEL_INT2_CFG_A            = 0x34,
        LSM303_REGISTER_ACCEL_INT2_SOURCE_A         = 0x35,
        LSM303_REGISTER_ACCEL_INT2_THS_A            = 0x36,
        LSM303_REGISTER_ACCEL_INT2_DURATION_A       = 0x37,
        LSM303_REGISTER_ACCEL_CLICK_CFG_A           = 0x38,
        LSM303_REGISTER_ACCEL_CLICK_SRC_A           = 0x39,
        LSM303_REGISTER_ACCEL_CLICK_THS_A           = 0x3A,
        LSM303_REGISTER_ACCEL_TIME_LIMIT_A          = 0x3B,
        LSM303_REGISTER_ACCEL_TIME_LATENCY_A        = 0x3C,
        LSM303_REGISTER_ACCEL_TIME_WINDOW_A         = 0x3D
    };


    //  Magnetometer registers

    enum
    {
        LSM303_REGISTER_MAG_CRA_REG_M             = 0x00,
        LSM303_REGISTER_MAG_CRB_REG_M             = 0x01,
        LSM303_REGISTER_MAG_MR_REG_M              = 0x02,
        LSM303_REGISTER_MAG_OUT_X_H_M             = 0x03,
        LSM303_REGISTER_MAG_OUT_X_L_M             = 0x04,
        LSM303_REGISTER_MAG_OUT_Z_H_M             = 0x05,
        LSM303_REGISTER_MAG_OUT_Z_L_M             = 0x06,
        LSM303_REGISTER_MAG_OUT_Y_H_M             = 0x07,
        LSM303_REGISTER_MAG_OUT_Y_L_M             = 0x08,
        LSM303_REGISTER_MAG_SR_REG_Mg             = 0x09,
        LSM303_REGISTER_MAG_IRA_REG_M             = 0x0A,
        LSM303_REGISTER_MAG_IRB_REG_M             = 0x0B,
        LSM303_REGISTER_MAG_IRC_REG_M             = 0x0C,
        LSM303_REGISTER_MAG_TEMP_OUT_H_M          = 0x31,
        LSM303_REGISTER_MAG_TEMP_OUT_L_M          = 0x32
    };





    //********************************************************************
    //
    //  Accelerometer stuff
    //
    //********************************************************************

    const float kGravitiesPerLeastSignificantBit    = 0.001;                    // When FS bit set to 0 (+/- 2G range setting)
    const float kEarthGravity                       = 9.80665;                  // Earth's gravity in m/s^2
    const float kConvertToMetersPerSec2             = kGravitiesPerLeastSignificantBit * kEarthGravity;  // (m/s^2)/G

    const float kConvertGaussToMicroTesla           = 100.0;                    // mT/G



    // Calibration constants for the magnetometer

    const int16_t kMagCalMinX          = -610;
    const int16_t kMagCalMinY          = -780;
    const int16_t kMagCalMinZ          = -485;

    const int16_t kMagCalMaxX          = 690;
    const int16_t kMagCalMaxY          = 500;
    const int16_t kMagCalMaxZ          = 625;

};







int LSM303DLHC::init()
{
    // Use I2C synchronously (no rush, more important to check for errors at each step)

    // Select the accelerometer update rate to 100Hz (updates every 10 ms)
    int err = I2cMaster::writeSync( kLSM303_AddressAccelerometer, LSM303_REGISTER_ACCEL_CTRL_REG1_A,
                               LSM303_ACCEL_DATA_RATE_100_Hz );

    if ( !err )
    {
        // Enable accelerometer high resolution mode
        err = I2cMaster::writeSync( kLSM303_AddressAccelerometer, LSM303_REGISTER_ACCEL_CTRL_REG4_A, 0b00001000 );

        if ( !err )
        {
            // Enable FIFO mode
            err = I2cMaster::writeSync( kLSM303_AddressAccelerometer, LSM303_REGISTER_ACCEL_CTRL_REG5_A, 0b01000000 );

            if ( !err )
            {
                // Set FIFO mode to stream
                err = I2cMaster::writeSync( kLSM303_AddressAccelerometer, LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A,
                                        0b10000000 );

                if ( !err )
                {
                    // Enable the magnetometer
                    err = I2cMaster::writeSync( kLSM303_AddessMagnetometer, LSM303_REGISTER_MAG_MR_REG_M,
                                          static_cast<uint8_t>( 0x00 ) );

                    if ( !err )
                    {
                        // Set magnetometer update rate to 30 Hz (updates every 33 ms)
                        err = I2cMaster::writeSync( kLSM303_AddessMagnetometer, LSM303_REGISTER_MAG_CRA_REG_M,
                                              LSM303_MAG_DATA_RATE_30_Hz );

                        if ( !err )
                        {
                            // Set the magnetometer gain to a known level
                            err = setMagGain( LSM303_MAGGAIN_1_3 );
                        }
                    }
                }
            }
        }
    }

    return err;
}



int LSM303DLHC::accelerometerUpdateRate()
{
    return 100;
}



// cppcheck-suppress unusedFunction
int LSM303DLHC::magnetometerUpdateRate()
{
    return 30;
}





#if 0
Vector3Float LSM303DLHC::getAccelerationG()
{
    return convertRawToG( getAccelerationRaw() );
}



void LSM303DLHC::getAccelerationMetersPerSec2()
{
    return convertRawToMetersPerSec2( getAccelerationRaw() );
}



Vector3Float getAccelerationCentimetersPerSec2()
{
    return convertRawToCentimetersPerSec2( getAccelerationRaw() );
}
#endif




Vector3Int LSM303DLHC::getAccelerationRaw()
{
    // In FIFO mode, can block-read up to 32 most recent values; only read 16.
    // At 200 Hz update rate for the accelerometer, 16 values is a sample
    // across 80 ms of time -- captures average across 64% of a 1/8 sec
    // CARRT navigation clock cycle.
    // At 100 Hz update rate, 12 values is a sample across 120 ms, almost a
    // perfect match for CARRT's 1/8 sec = 125ms navigation clock cycle
    // Each call to this function reliably timed at 3.13 ms for 16 values
    // Each call to this function reliably timed at 2.43 ms for 12 values

#define NBR_OF_ACCEL_VALUES         12

    union
    {
        uint8_t     values[NBR_OF_ACCEL_VALUES][6];     // order is xla, xha, yla, yha, zla, zha
        uint8_t     buffer[NBR_OF_ACCEL_VALUES * 6];
    } data;

#if 0
    int err = I2cMaster::readSync( kLSM303_AddressAccelerometer, (LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80),
                   (NBR_OF_ACCEL_VALUES * 6), data.buffer );

    if ( !err )
    {
        // Use vector of longs to avoid any risk of overflow
        Vector3Long temp( 0, 0, 0 );
        for ( int i = 0; i < NBR_OF_ACCEL_VALUES; ++i )
        {
            // Order is xla, xha, yla, yha, zla, zha
            // Combine high and low bytes, then shift right to discard lowest 4 bits (which are meaningless)
            // GCC performs an arithmetic right shift for signed negative numbers, but this code will not work
            // if you port it to a compiler that does a logical right shift instead.
            temp.x += static_cast<int16_t>( data.values[i][0] | static_cast<uint16_t>(data.values[i][1]) << 8 ) >> 4;
            temp.y += static_cast<int16_t>( data.values[i][2] | static_cast<uint16_t>(data.values[i][3]) << 8 ) >> 4;
            temp.z += static_cast<int16_t>( data.values[i][4] | static_cast<uint16_t>(data.values[i][5]) << 8 ) >> 4;
        }
#endif

#if 1
    volatile uint8_t nbrRead;
    volatile uint8_t status;

    int err = I2cMaster::readAsync( kLSM303_AddressAccelerometer, (LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80),
                   (NBR_OF_ACCEL_VALUES * 6), data.buffer, &nbrRead, &status );

    // Process data as it comes in...

    if ( !err )
    {
        while ( status == I2cMaster::kI2cNotStarted )
        {
            // Wait for our I2C session to start
        }

        // Use vector of longs to avoid any risk of overflow
        Vector3Long temp( 0, 0, 0 );
        for ( int i = 0; i < NBR_OF_ACCEL_VALUES; ++i )
        {
            while ( ( nbrRead / 6 < i + 1 ) && status != I2cMaster::kI2cError )
            {
                // Wait for the buffer to fill up enough
            }

            if ( status == I2cMaster::kI2cError )
            {
                // Special vector means error
                return Vector3Int( 1, 0, -1 );
            }

            // Order is xla, xha, yla, yha, zla, zha
            // Combine high and low bytes, then shift right to discard lowest 4 bits (which are meaningless)
            // GCC performs an arithmetic right shift for signed negative numbers, but this code will not work
            // if you port it to a compiler that does a logical right shift instead.
            temp.x += static_cast<int16_t>( data.values[i][0] | static_cast<uint16_t>(data.values[i][1]) << 8 ) >> 4;
            temp.y += static_cast<int16_t>( data.values[i][2] | static_cast<uint16_t>(data.values[i][3]) << 8 ) >> 4;
            temp.z += static_cast<int16_t>( data.values[i][4] | static_cast<uint16_t>(data.values[i][5]) << 8 ) >> 4;
        }
#endif

        // Divide by number of values to average.
        // Special case to replace division with faster bit-shift when possible
    #if NBR_OF_ACCEL_VALUES == 16
        temp >>= 4;
    #elif NBR_OF_ACCEL_VALUES == 8
        temp >>= 3;
    #elif NBR_OF_ACCEL_VALUES == 4
        temp >>= 2;
    #elif NBR_OF_ACCEL_VALUES == 2
        temp >>= 1;
    #else
        temp /= NBR_OF_ACCEL_VALUES;
    #endif

        return Vector3Int( temp.x, temp.y, temp.z );
    }
    else
    {
        // Special vector means error
        return Vector3Int( 1, 0, -1 );
    }
}


Vector3Float LSM303DLHC::convertRawToG( const Vector3Int& in )
{
    return Vector3Float
    (
        in.x * kGravitiesPerLeastSignificantBit,
        in.y * kGravitiesPerLeastSignificantBit,
        in.z * kGravitiesPerLeastSignificantBit
    );
}


Vector3Float LSM303DLHC::convertRawToMetersPerSec2( const Vector3Int& in )
{
    return Vector3Float
    (
        in.x * kConvertToMetersPerSec2,
        in.y * kConvertToMetersPerSec2,
        in.z * kConvertToMetersPerSec2
    );
}



// cppcheck-suppress unusedFunction
Vector2Float LSM303DLHC::convertRawToXYCentimetersPerSec2( const Vector3Int& in )
{
    return Vector2Float
    (
        in.x * kConvertToMetersPerSec2 * 100.0,
        in.y * kConvertToMetersPerSec2 * 100.0
    );
}



Vector3Float LSM303DLHC::convertRawToCentimetersPerSec2( const Vector3Int& in )
{
    return Vector3Float
    (
        in.x * kConvertToMetersPerSec2 * 100.0,
        in.y * kConvertToMetersPerSec2 * 100.0,
        in.z * kConvertToMetersPerSec2 * 100.0
    );
}



Vector2Float LSM303DLHC::convertRawToXYMetersPerSec2( const Vector3Int& in )
{
    return Vector2Float
    (
        in.x * kConvertToMetersPerSec2,
        in.y * kConvertToMetersPerSec2
    );
}



int LSM303DLHC::getAccelerationDataBlockSync( DataBlock* data, uint8_t nbr )
{
    // In FIFO mode, can block-read up to 32 most recent values.
    // Each call to this function with nbr = 32 reliably clocked at 5.74 ms.

    return I2cMaster::readSync( kLSM303_AddressAccelerometer, (LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80), nbr * 6, data->buffer );
}


// cppcheck-suppress unusedFunction
uint8_t LSM303DLHC::getAccelerationDataBlockAsync( volatile DataBlock* data, uint8_t nbrToRead,
                                                   volatile uint8_t* nbrRead, volatile uint8_t* status )
{
    // In FIFO mode, can block-read up to 32 most recent values.
    // Each call to this function with nbr = 32 reliably clocked at 5.74 ms.

    return I2cMaster::readAsync( kLSM303_AddressAccelerometer, (LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80),
                           nbrToRead * 6, data->buffer, nbrRead, status );
}


Vector3Int LSM303DLHC::convertDataBlockEntryToAccelerationRaw( const DataBlock& data, uint8_t item )
{
    // Order is xla, xha, yla, yha, zla, zha
    // Combine high and low bytes, then shift right to discard lowest 4 bits (which are meaningless)
    // GCC performs an arithmetic right shift for signed negative numbers, but this code will not work
    // if you port it to a compiler that does a logical right shift instead.
    return Vector3Int
    (
        static_cast<int16_t>( data.values[item][0] | static_cast<uint16_t>(data.values[item][1]) << 8 ) >> 4,
        static_cast<int16_t>( data.values[item][2] | static_cast<uint16_t>(data.values[item][3]) << 8 ) >> 4,
        static_cast<int16_t>( data.values[item][4] | static_cast<uint16_t>(data.values[item][5]) << 8 ) >> 4
    );
}


// cppcheck-suppress unusedFunction
Vector3Float LSM303DLHC::convertDataBlockEntryToAccelerationMetersPerSec2( const DataBlock& data, uint8_t i )
{
    // Order is xla, xha, yla, yha, zla, zha
    // Combine high and low bytes, then shift right to discard lowest 4 bits (which are meaningless)
    // GCC performs an arithmetic right shift for signed negative numbers, but this code will not work
    // if you port it to a compiler that does a logical right shift instead.
    return Vector3Int
    (
        ( static_cast<int16_t>( data.values[i][0] | static_cast<uint16_t>(data.values[i][1]) << 8 ) >> 4 ) * kConvertToMetersPerSec2,
        ( static_cast<int16_t>( data.values[i][2] | static_cast<uint16_t>(data.values[i][3]) << 8 ) >> 4 ) * kConvertToMetersPerSec2,
        ( static_cast<int16_t>( data.values[i][4] | static_cast<uint16_t>(data.values[i][5]) << 8 ) >> 4 ) * kConvertToMetersPerSec2
    );
}










int LSM303DLHC::setMagGain( LSM303MagnetometerGain gain )
{
    int err = I2cMaster::writeSync( kLSM303_AddessMagnetometer, LSM303_REGISTER_MAG_CRB_REG_M,
                              static_cast<uint8_t>( gain ) );

    if ( !err )
    {
        mMagnetometerGain = gain;
    }

    return err;
}


Vector3Int LSM303DLHC::getMagnetometerRaw()
{
    // Read the magnetometer (order is xh, xl, zh, zl, yh, yl)
    uint8_t data[6];
    int err = I2cMaster::readSync( kLSM303_AddessMagnetometer, LSM303_REGISTER_MAG_OUT_X_H_M, 6, data );

    if ( !err )
    {
        // Shift values to create properly formed integer (low byte first)
        return Vector3Int
        (
            static_cast<int16_t>( data[1] | static_cast<uint16_t>(data[0]) << 8 ),
            static_cast<int16_t>( data[5] | static_cast<uint16_t>(data[4]) << 8 ),
            static_cast<int16_t>( data[3] | static_cast<uint16_t>(data[2]) << 8 )
        );
    }
    else
    {
        return Vector3Int( 1, 0, -1 );
    }
}



#if 0
Vector3Float LSM303DLHC::getMagnetometerCalibrated()
{
    return( convertMagnetometerRawToCalibrated( getMagnetometerRaw() );
}


Vector3Float LSM303DLHC::getMagnetometerMicroTesla()
{
    return convertMagnetometerReadingToMicroTesla( getMagnetometerCalibrated() );
}


Vector3Float LSM303DLHC::convertMagnetometerRawToMicroTesla( const Vector3Int& in )
{
    return convertMagnetometerCalibratedToMicroTesla( convertMagnetometerRawToCalibrated( in ) );
}
#endif



#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

Vector3Float LSM303DLHC::convertMagnetometerCalibratedToMicroTesla( const Vector3Float& in )
{
    uint16_t gaussPerLsbXY;
    uint16_t gaussPerLsbZ;
    getGaussPerLsb( &gaussPerLsbXY, &gaussPerLsbZ );
    return Vector3Float
    (
        in.x * gaussPerLsbXY * kConvertGaussToMicroTesla,
        in.y * gaussPerLsbXY * kConvertGaussToMicroTesla,
        in.z * gaussPerLsbZ  * kConvertGaussToMicroTesla
    );
}

#pragma GCC diagnostic pop


Vector3Float LSM303DLHC::convertMagnetometerRawToCalibrated( const Vector3Int& in )
{
    return Vector3Float
    (
        ( in.x - kMagCalMinX ) / static_cast<float>( kMagCalMaxX - kMagCalMinX ) * 2.0 - 1.0,
        ( in.y - kMagCalMinY ) / static_cast<float>( kMagCalMaxY - kMagCalMinY ) * 2.0 - 1.0,
        ( in.z - kMagCalMinZ ) / static_cast<float>( kMagCalMaxZ - kMagCalMinZ ) * 2.0 - 1.0
    );
}



float LSM303DLHC::getHeading()
{
    Vector3Int m( getMagnetometerRaw() );
    Vector3Int a( getAccelerationRaw() );

    return calculateHeadingFromRawData( m, a );
}



float LSM303DLHC::calculateHeadingFromRawData( const Vector3Int& m, const Vector3Int& a )
{
    Vector3Float mf( convertMagnetometerRawToCalibrated( m ) );

    Vector3Float af( a );

    // Normalize
    af.normalize();

    // Compute E and N
    Vector3Float east = cross( mf, af );
    east.normalize();
    Vector3Float north = cross( af, east );

    // Compute heading
    Vector3Float ourDirection( 1, 0, 0 );
    float heading = atan2( east * ourDirection, north * ourDirection ) * 180 / M_PI;
    if ( heading < 0 )
    {
        heading += 360;
    }

    return heading;
}



void LSM303DLHC::getGaussPerLsb( uint16_t* xy, uint16_t* z )
{
    switch( mMagnetometerGain )
    {
        case LSM303_MAGGAIN_1_3:
            *xy = 1100;
            *z  = 980;
            break;

        case LSM303_MAGGAIN_1_9:
            *xy = 855;
            *z  = 760;
            break;

        case LSM303_MAGGAIN_2_5:
            *xy = 670;
            *z  = 600;
            break;

        case LSM303_MAGGAIN_4_0:
            *xy = 450;
            *z  = 400;
            break;

        case LSM303_MAGGAIN_4_7:
            *xy = 400;
            *z  = 255;
            break;

        case LSM303_MAGGAIN_5_6:
            *xy = 330;
            *z  = 295;
            break;

        case LSM303_MAGGAIN_8_1:
            *xy = 230;
            *z  = 205;
            break;
    }
}
