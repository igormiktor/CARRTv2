/*
    LSM303DLHC.h - Driver for the LSM303DLHC Accelerometer and Magnetometer/Compass
    used on CARRT (specifically it is the Adafruit LSM303DLHC Breakout board).

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




#ifndef LSM303DLHC_h
#define LSM303DLHC_h

#include <inttypes.h>


#include "Utils/VectorUtils.h"
#include "Drivers/NavSensorDataBlock.h"









namespace LSM303DLHC
{

    //  Magnetometer Gain Settings

    enum LSM303MagnetometerGain
    {
        LSM303_MAGGAIN_1_3                        = 0x20,  // +/- 1.3
        LSM303_MAGGAIN_1_9                        = 0x40,  // +/- 1.9
        LSM303_MAGGAIN_2_5                        = 0x60,  // +/- 2.5
        LSM303_MAGGAIN_4_0                        = 0x80,  // +/- 4.0
        LSM303_MAGGAIN_4_7                        = 0xA0,  // +/- 4.7
        LSM303_MAGGAIN_5_6                        = 0xC0,  // +/- 5.6
        LSM303_MAGGAIN_8_1                        = 0xE0   // +/- 8.1
    };


    int init();

    int accelerometerUpdateRate();  // in Hz

    Vector3Int getAccelerationRaw();
    Vector3Float convertRawToG( const Vector3Int& in );
    Vector3Float convertRawToMetersPerSec2( const Vector3Int& in );
    Vector2Float convertRawToXYMetersPerSec2( const Vector3Int& in );
    Vector3Float convertRawToCentimetersPerSec2( const Vector3Int& in );
    Vector2Float convertRawToXYCentimetersPerSec2( const Vector3Int& in );

    int getAccelerationDataBlockSync( DataBlock* data, uint8_t nbr = 32 );
    uint8_t getAccelerationDataBlockAsync( volatile DataBlock* data, uint8_t nbrToRead, volatile uint8_t* nbrRead,
                                           volatile uint8_t* status );
    Vector3Int convertDataBlockEntryToAccelerationRaw( const DataBlock& data, uint8_t item );
    Vector3Float convertDataBlockEntryToAccelerationMetersPerSec2( const DataBlock& data, uint8_t item );

    inline Vector3Float getAccelerationG()
    { return convertRawToG( getAccelerationRaw() ); }

    inline Vector3Float getAccelerationMetersPerSec2()
    { return convertRawToMetersPerSec2( getAccelerationRaw() ); }

    inline Vector3Float getAccelerationCentimetersPerSec2()
    { return convertRawToCentimetersPerSec2( getAccelerationRaw() ); }


    int setMagGain( LSM303MagnetometerGain gain );

    int magnetometerUpdateRate();  // in Hz

    Vector3Int getMagnetometerRaw();
    Vector3Float convertMagnetometerRawToCalibrated( const Vector3Int& in );
    Vector3Float convertMagnetometerCalibratedToMicroTesla( const Vector3Float& in );

    inline Vector3Float getMagnetometerCalibrated()
    { return convertMagnetometerRawToCalibrated( getMagnetometerRaw() ); }

    inline Vector3Float getMagnetometerMicroTesla()
    { return convertMagnetometerCalibratedToMicroTesla( getMagnetometerCalibrated() ); }

    inline Vector3Float convertMagnetometerRawToMicroTesla( const Vector3Int& in )
    { return convertMagnetometerCalibratedToMicroTesla( convertMagnetometerRawToCalibrated( in ) ); }

    float getHeading();
    float calculateHeadingFromRawData( const Vector3Int& magRaw, const Vector3Int& accelRaw );

};



#endif
