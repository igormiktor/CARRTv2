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




#ifndef L3GD20_h
#define L3GD20_h

#include <inttypes.h>

#include "Utils/VectorUtils.h"
#include "NavSensorDataBlock.h"



namespace L3GD20
{

    bool init();

    int gyroscopeUpdateRate();

    Vector3Int getAngularRatesRaw();
    Vector3Float convertRawToRadiansPerSecond( const Vector3Int& in );
    Vector3Float convertRawToDegreesPerSecond( const Vector3Int& in );
    float convertRawToRadiansPerSecond( int oneCoord );
    float convertRawToDegreesPerSecond( int oneCoord );

    Vector3Float getAngularRatesDegreesPerSecond();
    Vector3Float getAngularRatesRadiansPerSecond();

    void getAngularRatesDataBlockSync( DataBlock* data, uint8_t nbr );
    void getAngularRatesDataBlockAsync( DataBlock* data, uint8_t nbr, volatile uint8_t* nbrRead, volatile uint8_t* status );
    Vector3Int convertDataBlockEntryToAngularRatesRaw( const DataBlock& data, uint8_t item );
    Vector3Float convertDataBlockEntryToAngularRatesRadiansPerSecond( const DataBlock& d, uint8_t item );

};


#endif
