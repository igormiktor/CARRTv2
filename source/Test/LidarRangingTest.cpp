/*
    LidarRangingTest.cpp - Test of how the lidar ranges across various
    configurations, azimuths, and map-making.

    Copyright (c) 2018 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; withlaptop even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/I2cMaster.h"
#include "AVRTools/USART0.h"

#include "Drivers/Lidar.h"

#include "NavigationMap.h"





void respondToInput();
void doPing();
void doScanIncrement( char* token );
void doMapRescale( char* token );
void doUpdateScale( int global, int local );
void doUpdateLidarMode( int pingSize );
void doInstructions();
void doMapScan();
void executeMapScan();
void doSlew( char* token );
void doConfigScan();
void doCalibrate();
int getCalibrateInput();
void doCalibrationScan();



Serial0 laptop;

Lidar::Configuration gLidarMode;
int gGlobalCmPerGrid;
int gLocalCmPerGrid;
int gScanIncrement;






int main()
{
    initSystem();
    initSystemClock();

    I2cMaster::start();

    gLidarMode = Lidar::kDefault;
    gGlobalCmPerGrid = 32;
    gLocalCmPerGrid = 16;
    gScanIncrement = 2;

    laptop.start( 115200 );

    delayMilliseconds( 1000 );

    Lidar::init();

    delayMilliseconds( 1000 );

    doInstructions();

    while ( 1 )
    {
        if ( laptop.available() )
        {
            respondToInput();
        }
        else
        {
            delayMilliseconds( 250 );
        }
    }
}



void doInstructions()
{
    laptop.println( "Lidar basic ranging test..." );
    laptop.println( "Enter a (or A) followed by relative azimuth to slew the radar" );
    laptop.println( "Enter p (or P) to ping the lidar" );
    laptop.println( "Enter s (or S) for an azimuth scan" );
    laptop.println( "Enter x (or X) to conduct a configuration scan" );
    laptop.println( "Enter i (or I) followed by the scan increment (in deg)" );
    laptop.println( "Enter c (or C) followed by nbr set a lidar configuration" );
    laptop.println( "Enter n (or N) to enter calibration mode" );
    laptop.println( "Enter m (or M) followed by global scale and local scale (cm) to reset the Navigation map" );
}




void doPing()
{
    int rng;

    int err = Lidar::getDistanceInCm( &rng );

    if ( !err )
    {
        laptop.print( "Range:  " );
        laptop.print( rng );
        laptop.println();
    }
    else
    {
        laptop.print( "Range attempt produced an error " );
        laptop.println( err );
    }
}



void doLidarModeChange( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int mode = atoi( token );

        if ( mode >= Lidar::kDefault && mode <= Lidar::kLowSensitivityButLowerError )
        {
            doUpdateLidarMode( mode );
        }
    }
}



void doUpdateLidarMode( int mode )
{
    static const char* modeStr[] =
    {
        "Default",
        "Short range, fast",
        "Shorter range, fastest",
        "Default range, faster at short range",
        "Maximum range",
        "High sensitivity, high error",
        "Low sensitivity, low error"
    };


    gLidarMode = static_cast<Lidar::Configuration>( mode );

    int err = Lidar::setConfiguration( gLidarMode );

    if ( err )
    {
        laptop.print( "Lidar error " );
        laptop.print( err );
        laptop.println( " when setting configuration" );
    }
    else
    {
        laptop.print( "Lidar mode:  " );
        laptop.println( modeStr[mode] );
    }
}


void doConfigScan()
{
    laptop.println( "Config Mode Scan..." );

    for ( int i = Lidar::kDefault; i <= Lidar::kLowSensitivityButLowerError; ++i )
    {
        laptop.print( 'M' );
        laptop.print( i );
        laptop.print( ",   " );
    }
    laptop.println();

    doCalibrationScan();

    int err = Lidar::setConfiguration( Lidar::kDefault );
    if ( err )
    {
        laptop.print( "Error restoring default configureation " );
        laptop.println( err );
    }
}



void doScanIncrement( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int scanIncrement = atoi( token );

        if ( scanIncrement > 0 && scanIncrement < 21 )
        {
            gScanIncrement = scanIncrement;
            laptop.print( "Scan increment (deg):  " );
            laptop.println( gScanIncrement );
        }
    }
}



void doMapRescale( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int globalScale = atoi( token );

        token = strtok( 0, " \t" );
        if (  token )
        {
            int localScale = atoi( token );

            if ( globalScale > 0 && localScale > 0 )
            {
                doUpdateScale( globalScale, localScale );
            }
        }
    }
}



void doUpdateScale( int global, int local )
{
    gGlobalCmPerGrid = global;
    gLocalCmPerGrid = local;

    laptop.print( "Global cm/grid:  " );
    laptop.print( gGlobalCmPerGrid );
    laptop.print( "     Local cm/grid:  " );
    laptop.println( gLocalCmPerGrid );
}



void doMapScan()
{
    NavigationMap::init( gGlobalCmPerGrid, gLocalCmPerGrid );

    executeMapScan();
}



void executeMapScan()
{
    laptop.println( "Lidar mapping scan..." );
    laptop.println( "Angle,      Distance,      X,      Y" );

    const float deg2rad = M_PI/180.0;

    for ( int slewAngle = -80; slewAngle < 81; slewAngle += gScanIncrement )
    {
        Lidar::slew( slewAngle );
        delayMilliseconds( 500 );

        int d;
        int err = Lidar::getMedianDistanceInCm( &d );

        laptop.print( slewAngle );
        laptop.print( ",       " );

        if ( !err )
        {
            laptop.print( d );

            // Record this observation
            float rad = deg2rad * slewAngle;
            float x = static_cast<float>( d ) * cos( rad );
            float y = static_cast<float>( d ) * sin( rad );

            laptop.print( ",         " );
            laptop.print( static_cast<int>( x + 0.5 ) );
            laptop.print( ",         " );
            laptop.print( static_cast<int>( y + 0.5 ) );

            NavigationMap::markObstacle( x + 0.5, y + 0.5 );
        }
        else
        {
            laptop.print( ", , , " );
            laptop.print( err );
        }

        laptop.println();
    }
    laptop.println();

    Lidar::slew( 0 );

    // Output the results

    laptop.print( "Global cm/grid:  " );
    laptop.print( gGlobalCmPerGrid );
    laptop.print( "     Local cm/grid:  " );
    laptop.println( gLocalCmPerGrid );
    laptop.println();

    char* globalMapOut = NavigationMap::getGlobalMap().dumpToStr();
    laptop.println( globalMapOut );
    free( globalMapOut );

    char* localMapOut = NavigationMap::getLocalMap().dumpToStr();
    laptop.println( localMapOut );
    free( localMapOut );

    laptop.println( "\n**** End Map ****\n" );
}




void doSlew( char* token )
{
    token = strtok( 0, " \t" );

    if ( token )
    {
        int bearing = atoi( token );

        if ( -80 <= bearing && bearing <= 80 )
        {
            Lidar::slew( bearing );
            laptop.print( "Ladar slewed to azimuth:  " );
            laptop.println( bearing );

            doPing();
        }
    }
}




void doCalibrate()
{

    laptop.println( "Calibration Mode: enter correct range or enter -1 or e to stop..." );

    laptop.print( "R,  " );
    for ( int i = Lidar::kDefault; i <= Lidar::kLowSensitivityButLowerError; ++i )
    {
        laptop.print( 'M' );
        laptop.print( i );
        laptop.print( ",   " );
    }
    laptop.println();

    while ( 1 )
    {
        if ( laptop.available() )
        {
            int correctRng = getCalibrateInput();
            if ( correctRng < 0 )
            {
                break;
            }
            else if ( correctRng > 0 )
            {
                laptop.print( correctRng );
                laptop.print( ",   " );
                doCalibrationScan();
            }
        }
        else
        {
            delayMilliseconds( 250 );
        }
    }

    laptop.println( "End of calibration run" );


    int err = Lidar::setConfiguration( Lidar::kDefault );
    if ( err )
    {
        laptop.print( "Error restoring default configureation " );
        laptop.println( err );
    }
}



int getCalibrateInput()
{
    char input[81];

    laptop.readLine( input, 80 );

    char* token;
    token = strtok( input, " \t" );

    if ( token )
    {
        if ( *token == 'e' )
        {
            return -1;
        }
        else
        {
            return atoi( token );
        }
    }
    else
    {
        return 0;
    }
}



void doCalibrationScan()
{
    for ( int i = Lidar::kDefault; i <= Lidar::kLowSensitivityButLowerError; ++i )
    {
        int err = Lidar::setConfiguration( static_cast<Lidar::Configuration>( i ) );

        if ( err )
        {
            laptop.print( "E" );
            laptop.print( err );
        }
        else
        {
            delayMilliseconds( 10 );
            int rng;
            int err = Lidar::getDistanceInCm( &rng );
            if ( err )
            {
                laptop.print( "e" );
                laptop.print( err );
            }
            else
            {
                laptop.print( rng );
            }
        }
        laptop.print( ",   " );
    }
    laptop.println();
}




void respondToInput()
{
    char input[81];

    laptop.readLine( input, 80 );

    char* token;
    token = strtok( input, " \t" );

    if ( token )
    {
        switch ( *token )
        {
            case 'a':
            case 'A':
                doSlew( token );
                break;

            case 'p':
            case 'P':
                doPing();
                break;

            case 'c':
            case 'C':
                doLidarModeChange( token );
                break;

            case 'x':
            case 'X':
                doConfigScan();
                break;

            case 's':
            case 'S':
                doMapScan();
                break;

            case 'i':
            case 'I':
                doScanIncrement( token );
                break;

            case 'm':
            case 'M':
                doMapRescale( token );
                break;

            case 'n':
            case 'N':
                doCalibrate();
                break;
        }
    }
}








