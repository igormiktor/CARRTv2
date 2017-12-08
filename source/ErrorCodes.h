/*
    ErrorCodes.h - Error codes for CARRT

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





#ifndef ErrorCodes_h
#define ErrorCodes_h


enum ErrorCodes
{
    kNoError                        = 0,

    // System errors
    kSystemError                    = 100,
    kEventQueueOverflowError        = 101,
    kNullStateError                 = 102,
    kBadHeadingError                = 103,
    kNoReturnStateError             = 104,
    kUnconstrainedDriveError        = 105,
    kBadlyConstrainedDriveError     = 106,

    // Battery problems
    kMotorBatteryLowError           = 201,
    kCpuBatteryLowError             = 202,

    // Out-of-Memory
    kOutOfMemoryError               = 301,

    // Programming problems
    kNullMenuToStateMapFunc         = 401,
    kNullStateToChangeState         = 402,
    kNullStateInProgram             = 403,

    // Program Drive problems
    kPgmDriveNextStateNull          = 501,


    kTestError1                     = 901,
    kTestError2                     = 902,

    kOtherError                     = 999
};



#endif
