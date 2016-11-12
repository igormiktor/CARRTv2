/*
    L3GD20.h - Driver for the L3GD20 Gyroscope used on CARRT 
    (specifically it is the Adafruit L3GD20 Breakout board).
    
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




#include "L3GD20.h"

#include <inttypes.h>

#include "AVRTools/I2cMaster.h"
// #include "RoverGlobals.h"


enum
{
    L3DG20_RANGE_250DPS,
    L3DG20_RANGE_500DPS,
    L3DG20_RANGE_2000DPS
};


//  Gyroscope registers

enum
{                                                 // DEFAULT    TYPE
    L3GD20_REGISTER_WHO_AM_I            = 0x0F,   // 11010100   r
    L3GD20_REGISTER_CTRL_REG1           = 0x20,   // 00000111   rw
    L3GD20_REGISTER_CTRL_REG2           = 0x21,   // 00000000   rw
    L3GD20_REGISTER_CTRL_REG3           = 0x22,   // 00000000   rw
    L3GD20_REGISTER_CTRL_REG4           = 0x23,   // 00000000   rw
    L3GD20_REGISTER_CTRL_REG5           = 0x24,   // 00000000   rw
    L3GD20_REGISTER_REFERENCE           = 0x25,   // 00000000   rw
    L3GD20_REGISTER_OUT_TEMP            = 0x26,   //            r
    L3GD20_REGISTER_STATUS_REG          = 0x27,   //            r
    L3GD20_REGISTER_OUT_X_L             = 0x28,   //            r
    L3GD20_REGISTER_OUT_X_H             = 0x29,   //            r
    L3GD20_REGISTER_OUT_Y_L             = 0x2A,   //            r
    L3GD20_REGISTER_OUT_Y_H             = 0x2B,   //            r
    L3GD20_REGISTER_OUT_Z_L             = 0x2C,   //            r
    L3GD20_REGISTER_OUT_Z_H             = 0x2D,   //            r
    L3GD20_REGISTER_FIFO_CTRL_REG       = 0x2E,   // 00000000   rw
    L3GD20_REGISTER_FIFO_SRC_REG        = 0x2F,   //            r
    L3GD20_REGISTER_INT1_CFG            = 0x30,   // 00000000   rw
    L3GD20_REGISTER_INT1_SRC            = 0x31,   //            r
    L3GD20_REGISTER_TSH_XH              = 0x32,   // 00000000   rw
    L3GD20_REGISTER_TSH_XL              = 0x33,   // 00000000   rw
    L3GD20_REGISTER_TSH_YH              = 0x34,   // 00000000   rw
    L3GD20_REGISTER_TSH_YL              = 0x35,   // 00000000   rw
    L3GD20_REGISTER_TSH_ZH              = 0x36,   // 00000000   rw
    L3GD20_REGISTER_TSH_ZL              = 0x37,   // 00000000   rw
    L3GD20_REGISTER_INT1_DURATION       = 0x38    // 00000000   rw
};



// Data rates and cut-off

#define     L3DG20_DATA_RATE_95_Hz_CUTOFF_12p5_Hz       0b00001111
#define     L3DG20_DATA_RATE_95_Hz_CUTOFF_25_Hz         0b00011111
// #define     L3DG20_DATA_RATE_95_Hz_CUTOFF_25_Hz         0b00101111
// #define     L3DG20_DATA_RATE_95_Hz_CUTOFF_25_Hz         0b00111111
#define     L3DG20_DATA_RATE_190_Hz_CUTOFF_12p5_Hz      0b01001111
#define     L3DG20_DATA_RATE_190_Hz_CUTOFF_25_Hz        0b01011111
#define     L3DG20_DATA_RATE_190_Hz_CUTOFF_50_Hz        0b01101111
#define     L3DG20_DATA_RATE_190_Hz_CUTOFF_70_Hz        0b01111111
#define     L3DG20_DATA_RATE_380_Hz_CUTOFF_20_Hz        0b10001111
#define     L3DG20_DATA_RATE_380_Hz_CUTOFF_25_Hz        0b10011111
#define     L3DG20_DATA_RATE_380_Hz_CUTOFF_50_Hz        0b10101111
#define     L3DG20_DATA_RATE_380_Hz_CUTOFF_100_Hz       0b10111111
#define     L3DG20_DATA_RATE_760_Hz_CUTOFF_30_Hz        0b11001111
#define     L3DG20_DATA_RATE_760_Hz_CUTOFF_35_Hz        0b11011111
#define     L3DG20_DATA_RATE_760_Hz_CUTOFF_50_Hz        0b11101111
#define     L3DG20_DATA_RATE_760_Hz_CUTOFF_100_Hz       0b11111111


namespace
{
	//  I2C Address

	const uint8_t kL3GD20_Address           = 0x6B;        // 1101001
	const uint8_t kL3GD20_Id                = 0b11010100;

	// Key constants

	const float kL3GD20_SENSITIVITY_250DPS      = 0.00875F;
	const float kL3GD20_SENSITIVITY_500DPS      = 0.0175F;
	const float kL3GD20_SENSITIVITY_2000DPS     = 0.070F;
	const float kL3GD20_DPS_TO_RADS             = 0.017453293F;     // degrees/s to rad/s multiplier

	const float kL3GD20_TO_RADS_250DPS          = kL3GD20_SENSITIVITY_250DPS * kL3GD20_DPS_TO_RADS;
	const float kL3GD20_TO_RADS_500DPS          = kL3GD20_SENSITIVITY_500DPS * kL3GD20_DPS_TO_RADS;
	const float kL3GD20_TO_RADS_2000DPS         = kL3GD20_SENSITIVITY_2000DPS * kL3GD20_DPS_TO_RADS;
}






bool L3GD20::init()
{
    // Check we are talking to the right device
    uint8_t id;
    bool err = I2cMaster::readSync( kL3GD20_Address, L3GD20_REGISTER_WHO_AM_I, 1, &id );
    if ( err || id != kL3GD20_Id )
    {
        return false;
    }

    // Turn on the L3GD20: all 3 axes enabled, with selected output data rate
    // 190 Hz data rate means an update every 5.26 ms
    err = I2cMaster::writeSync( kL3GD20_Address, L3GD20_REGISTER_CTRL_REG1, L3DG20_DATA_RATE_190_Hz_CUTOFF_25_Hz );

    if ( !err )
    {
        // Set scale to 250dps
        err = I2cMaster::writeSync( kL3GD20_Address, L3GD20_REGISTER_CTRL_REG4, static_cast<uint8_t>( 0x00 ) );

        if ( !err )
        {
            // Enable FIFO
            err = I2cMaster::writeSync( kL3GD20_Address, L3GD20_REGISTER_CTRL_REG5, 0b01000000 );

            if ( !err )
            {
                // Set FIFO stream mode
                err = I2cMaster::writeSync( kL3GD20_Address, L3GD20_REGISTER_FIFO_CTRL_REG, 0b01000000 );

            }
        }
    }

    return err;
}



int L3GD20::gyroscopeUpdateRate()
{
    return 190;
}





Vector3Int L3GD20::getAngularRatesRaw()
{
    // In FIFO mode, can block-read up to 32 most recent values.
    // At 190 Hz update rate for the accelerometer, 16 values is a sample
    // across 84 ms of time; 23 values span 121 ms of time (close to 125ms nav update cycle).
    // Each call to this function reliably clocked at 3.13 ms for 16 values (synchronous version).
    // Each call to this function reliably clocked at 2.40 ms for 12 values (synchronous version).

#define NBR_OF_GYRO_VALUES         23		

    union
    {
        uint8_t     values[NBR_OF_GYRO_VALUES][6];     // order is xlo, xhi, ylo, yhi, zlo, zhi
        uint8_t     buffer[NBR_OF_GYRO_VALUES * 6];
    } data;

#if 0   // Read data synchronously

    I2cMaster::readSync( kL3GD20_Address, (L3GD20_REGISTER_OUT_X_L | 0x80), (NBR_OF_GYRO_VALUES * 6), data.buffer );

    if ( !err )
    {
        // Use vector of longs to avoid any risk of overflow
        Vector3Long temp( 0, 0, 0 );
        for ( int i = 0; i < NBR_OF_GYRO_VALUES; ++i )
        {
            // Order is xlo, xhi, ylo, yhi, zlo, zhi
            temp.x += static_cast<int16_t>( data.values[i][0] | data.values[i][1] << 8 );
            temp.y += static_cast<int16_t>( data.values[i][2] | data.values[i][3] << 8 );
            temp.z += static_cast<int16_t>( data.values[i][4] | data.values[i][5] << 8 );
        }
        
#endif


#if 1	// Read data asynchronously

    volatile uint8_t nbrRead;
    volatile uint8_t status;

    int err = I2cMaster::readAsync( kL3GD20_Address, (L3GD20_REGISTER_OUT_X_L | 0x80), (NBR_OF_GYRO_VALUES * 6),
                    data.buffer, &nbrRead, &status );

    // Process data as it comes in...

    if ( !err )
    {
        while ( status == I2cMaster::kI2cNotStarted )
        {
            // Wait for our I2C session to start
        }

        // Use vector of longs to avoid any risk of overflow
        Vector3Long temp( 0, 0, 0 );
        for ( int i = 0; i < NBR_OF_GYRO_VALUES; ++i )
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

            // Order is xlo, xhi, ylo, yhi, zlo, zhi
            temp.x += static_cast<int16_t>( data.values[i][0] | data.values[i][1] << 8 );
            temp.y += static_cast<int16_t>( data.values[i][2] | data.values[i][3] << 8 );
            temp.z += static_cast<int16_t>( data.values[i][4] | data.values[i][5] << 8 );
        }
#endif

        // Divide by number of values to average.
        // Special case to replace division with faster bit-shift when possible
    #if NBR_OF_GYRO_VALUES == 16
        temp >>= 4;
    #elif NBR_OF_GYRO_VALUES == 8
        temp >>= 3;
    #elif NBR_OF_GYRO_VALUES == 4
        temp >>= 2;
    #elif NBR_OF_GYRO_VALUES == 2
        temp >>= 1;
    #else
        temp /= NBR_OF_GYRO_VALUES;
    #endif

        return Vector3Int( temp.x, temp.y, temp.z );
    }
    else
    {
        // Special vector means error
        return Vector3Int( 1, 0, -1 );
    }
}




Vector3Float L3GD20::getAngularRatesDegreesPerSecond()
{
    Vector3Float tmp( getAngularRatesRaw() );
    return tmp * kL3GD20_SENSITIVITY_250DPS;
}


Vector3Float L3GD20::getAngularRatesRadiansPerSecond()
{
    Vector3Float tmp( getAngularRatesRaw() );
    return tmp * kL3GD20_TO_RADS_250DPS;
}


Vector3Float L3GD20::convertRawToRadiansPerSecond( const Vector3Int& in )
{
    return Vector3Float
    (
        in.x * kL3GD20_TO_RADS_250DPS,
        in.y * kL3GD20_TO_RADS_250DPS,
        in.z * kL3GD20_TO_RADS_250DPS
    );
}


Vector3Float L3GD20::convertRawToDegreesPerSecond( const Vector3Int& in )
{
    return Vector3Float
    (
        in.x * kL3GD20_SENSITIVITY_250DPS,
        in.y * kL3GD20_SENSITIVITY_250DPS,
        in.z * kL3GD20_SENSITIVITY_250DPS
    );
}


float L3GD20::convertRawToDegreesPerSecond( int oneCoord )
{
    return oneCoord * kL3GD20_SENSITIVITY_250DPS;
}



float L3GD20::convertRawToRadiansPerSecond( int oneCoord )
{
    return oneCoord * kL3GD20_TO_RADS_250DPS;
}



void L3GD20::getAngularRatesDataBlockSync( DataBlock* data, uint8_t nbr )
{
    // In FIFO mode, can block-read up to 32 most recent values.
    // Each call to this function with nbr = 32 reliably clocked at 5.73 ms.

    I2cMaster::readSync( kL3GD20_Address, (L3GD20_REGISTER_OUT_X_L | 0x80), nbr * 6, data->buffer );
}


void L3GD20::getAngularRatesDataBlockAsync( DataBlock* data, uint8_t nbr, volatile uint8_t* nbrRead, volatile uint8_t* status )
{
    // In FIFO mode, can block-read up to 32 most recent values.

    I2cMaster::readAsync( kL3GD20_Address, (L3GD20_REGISTER_OUT_X_L | 0x80), nbr * 6, data->buffer, nbrRead, status );
}


Vector3Int L3GD20::convertDataBlockEntryToAngularRatesRaw( const DataBlock& data, uint8_t item )
{
    return Vector3Int
    (
        // Order is xlo, xhi, ylo, yhi, zlo, zhi
        static_cast<int16_t>( data.values[item][0] | data.values[item][1] << 8 ),
        static_cast<int16_t>( data.values[item][2] | data.values[item][3] << 8 ),
        static_cast<int16_t>( data.values[item][4] | data.values[item][5] << 8 )
    );
}


Vector3Float L3GD20::convertDataBlockEntryToAngularRatesRadiansPerSecond( const DataBlock& data, uint8_t item )
{
    return Vector3Float
    (
        // Order is xlo, xhi, ylo, yhi, zlo, zhi
        ( static_cast<int16_t>( data.values[item][0] | data.values[item][1] << 8 ) ) * kL3GD20_TO_RADS_250DPS,
        ( static_cast<int16_t>( data.values[item][2] | data.values[item][3] << 8 ) ) * kL3GD20_TO_RADS_250DPS,
        ( static_cast<int16_t>( data.values[item][4] | data.values[item][5] << 8 ) ) * kL3GD20_TO_RADS_250DPS
    );
}








