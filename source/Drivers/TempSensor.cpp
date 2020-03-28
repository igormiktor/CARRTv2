/*
    TempSensor.cpp - Functions for reading the temperature sensor.

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





#include "TempSensor.h"

#include "CarrtPins.h"

#include "AVRTools/Analog2Digital.h"



float TempSensor::getTempC()
{
    // Need to convert the raw temp to a Centigrade value
    // This assumes the voltage reference is set to 2.56 V
    const float kAnalogToVolts = 2.56/1023.0;

    int rawVoltage = 0;
    for ( int i = 0; i < 4; ++i )
    {
        rawVoltage += readGpioPinAnalog( pTemperatureSensorPin );
    }

    //  rawVoltage /= 4;
    rawVoltage >>= 2;         // divide by 4 using bit-shift
    float voltage = rawVoltage * kAnalogToVolts;
    float degC = (voltage - 0.5) * 100.0;
    return degC;
}




float TempSensor::getTempF()
{
    return getTempC() * 9.0/5.0 + 32.0;
}

