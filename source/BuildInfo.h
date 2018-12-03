/*
    BuildInfo.h - Contains build information for CARRT

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





#ifndef BuildInfo_h
#define BuildInfo_h





/*
 * Convert the CARRT version number into a string using only the preprocessor
 *
 */


#define CARRT_VERSION           " v" CARRT_VERSION_STR




/*
 * Determine the features compiled into this CARRT build and encapsulate that as
 * a string; all done in the preprocessor
 *
 */


#if CARRT_INCLUDE_TESTS_IN_BUILD
#define CARRT_TEST_STR  "T"
#else
#define CARRT_TEST_STR  ""
#endif

#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD
#define CARRT_PROGDRV_STR  "P"
#else
#define CARRT_PROGDRV_STR  ""
#endif

#if CARRT_INCLUDE_GOTODRIVE_IN_BUILD
#define CARRT_GOTODRV_STR  "G"
#else
#define CARRT_GOTODRV_STR  ""
#endif

#if CARRT_NAVIGATE_USING_INERTIAL
#define CARRT_NAV_STR  "IMU"
#elif CARRT_NAVIGATE_USING_DEADRECKONING
#define CARRT_NAV_STR  "DR"
#else
#error "One of CARRT_NAVIGATE_USING_INERTIAL or CARRT_NAVIGATE_USING_DEADRECKONING must be defined."
#endif

#if CARRT_ENABLE_DEBUG_SERIAL
#define CARRT_DEBUG_STR  "D"
#else
#define CARRT_DEBUG_STR  ""
#endif


#define CARRT_FEATURES   " " CARRT_GOTODRV_STR " " CARRT_PROGDRV_STR " " CARRT_TEST_STR " " CARRT_NAV_STR " " CARRT_DEBUG_STR





/*
 * Following code transforms the __DATE__ macro from Mmm [D]D YYYY format to my
 * preferred format of YYYYMMDD.  So that Dec  2 2018 becomes 20181202.
 *
 * This is a complicated as it is because the transformation is done
 * entirely in the preprocessor, with no run-time or memory overhead.
 *
 */


#define CARRT_BUILD_YEAR    __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10]

#define DAY_FIRST_DIGIT     (__DATE__[4] == ' ') ? '0' : __DATE__[4]

#define CARRT_BUILD_DAY     DAY_FIRST_DIGIT, __DATE__[5]


#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')


#define CARRT_BUILD_MONTH_FIRST_DIGIT \
    ( \
        (BUILD_MONTH_IS_JAN) ? '0' : \
        (BUILD_MONTH_IS_FEB) ? '0' : \
        (BUILD_MONTH_IS_MAR) ? '0' : \
        (BUILD_MONTH_IS_APR) ? '0' : \
        (BUILD_MONTH_IS_MAY) ? '0' : \
        (BUILD_MONTH_IS_JUN) ? '0' : \
        (BUILD_MONTH_IS_JUL) ? '0' : \
        (BUILD_MONTH_IS_AUG) ? '0' : \
        (BUILD_MONTH_IS_SEP) ? '0' : \
        (BUILD_MONTH_IS_OCT) ? '1' : \
        (BUILD_MONTH_IS_NOV) ? '1' : \
        (BUILD_MONTH_IS_DEC) ? '1' : \
        /* error default */    '9' \
    )


#define CARRT_BUILD_MONTH_SECOND_DIGIT \
    ( \
        (BUILD_MONTH_IS_JAN) ? '1' : \
        (BUILD_MONTH_IS_FEB) ? '2' : \
        (BUILD_MONTH_IS_MAR) ? '3' : \
        (BUILD_MONTH_IS_APR) ? '4' : \
        (BUILD_MONTH_IS_MAY) ? '5' : \
        (BUILD_MONTH_IS_JUN) ? '6' : \
        (BUILD_MONTH_IS_JUL) ? '7' : \
        (BUILD_MONTH_IS_AUG) ? '8' : \
        (BUILD_MONTH_IS_SEP) ? '9' : \
        (BUILD_MONTH_IS_OCT) ? '0' : \
        (BUILD_MONTH_IS_NOV) ? '1' : \
        (BUILD_MONTH_IS_DEC) ? '2' : \
        /* error default */    '9' \
    )

#define CARRT_BUILD_DATE    { ' ', CARRT_BUILD_YEAR, CARRT_BUILD_MONTH_FIRST_DIGIT, CARRT_BUILD_MONTH_SECOND_DIGIT, CARRT_BUILD_DAY, 0 }

#define CARRT_BUILD_TIME    " " __TIME__


#endif  // BuildInfo_h

