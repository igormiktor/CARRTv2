/*
    Battery.h - Functions for checking bettery levels
    
    Copyright (c) 2018 Igor Mikolic-Torreira.  All right reserved.

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





#ifndef Battery_h
#define Battery_h



// These functions require initializing the analog-to-digital
// module to use 2.56V as the reference voltage



namespace Battery
{
    
    enum
    {
        kBatteriesOkay = 0,
        kMotorBatteryCriticalLow,
        kCpuBatteryCriticalLow
    };

    void initBatteryStatusDisplay();

    bool isChargerConnected();

    int getCpuBatteryMilliVoltage();
    int getMotorBatteryMilliVoltage();

    bool isMotorBatteryOkay( int milliVolts );
    bool isCpuBatteryOkay( int milliVolts );

    void displayMotorBatteryStatusLed( int milliVolts );
    void displayCpuBatteryStatusLed( int milliVolts );

    int checkAndDisplayBatteryStatus();

};

#endif
