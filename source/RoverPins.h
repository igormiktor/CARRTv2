/*
    RoverPins.h - Defines the ArduinoMega2560 pins used by CARRT
    
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



#ifndef RoverPins_h
#define RoverPins_h

#include "AVRTools/ArduinoMegaPins.h"


// *** Macro definitions to facilitate direct access to AVR pins


// Analog pins
#define pBatteryMotorVoltagePin             pPinA00    // Arduino pin A0, PF0 ( ADC0 )
#define pBatteryElectronicsVoltagePin       pPinA01    // Arduino pin A1, PF1 ( ADC1 )
#define pTemperatureSensorPin               pPinA02    // Arduino pin A2, PF2 ( ADC2 )

// Digital pins
#define pNotUsedInterruptPin                pPin02     // Arduino pin 02, PE4 ( OC3B/INT4 )

#define pBeepPin                            pPin13     // Arduino pin 13, PB7 ( OC0A/OC1C/PCINT7 )

#define pBatteryMotorLedGreenPin            pPin22     // Arduino pin 22, PA0 ( AD0 )
#define pBatteryMotorLedYellowPin           pPin24     // Arduino pin 24, PA2 ( AD2 )
#define pBatteryMotorLedRedPin              pPin26     // Arduino pin 26, PA4 ( AD4 )

#define pBatteryElectronicsLedGreenPin      pPin23     // Arduino pin 23, PA1 ( AD1 )
#define pBatteryElectronicsLedYellowPin     pPin25     // Arduino pin 25, PA3 ( AD3 )
#define pBatteryElectronicsLedRedPin        pPin27     // Arduino pin 27, PA5 ( AD5 )

#define pBatteryMotorChargerConnectedPin    pPin28     // Arduino pin 28, PA6 ( AD6 )

#define pRangeSensorDataPin                 pPin29     // Arduino pin 29, PA7 ( AD7 )







#endif


