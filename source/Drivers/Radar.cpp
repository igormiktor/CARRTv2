#include "Radar.h"

#include "AVRTools/GpioPinMacros.h"
#include "AVRTools/SystemClock.h"

#include "RoverPins.h"

#include "TempSensor.h"

#include "ServoDriver.h"


/*
 * Designed as a namespace instead of a class because there is only one
 * radar and need to access the radar from various places in CARRT.
 * 
 * A class requires a single global instance; this way we can access
 * strictly through the function interface without an instances
 * 
 * Note: this means this driver code is only good for ONE radar.
 * Use of multiple radars would require conversion to a class
 */



// Add some "local" functions and constants to the namespace

namespace Radar
{

    // Following values tailored to Parallax PING sensor

    const int kMaxEchoTime                      = 18000;    // microseconds

    const int kDurationOfTriggerPulse           = 5;        // microseconds

    const int kDelayBetweenPings                = 25;       // MILLIseconds

    // Maximum uS it takes for sensor to start the ping.
    // Parallax PING sensor docs say "holdoff" is 750 us.
    const unsigned long kMaxSensorDelay         = 800;      // microseconds



    unsigned long mPingStartTime;
    int8_t mCurrentAngle;

    uint16_t convertToPulseLenFromDegreesRelative( int8_t degrees );
    unsigned int convertEchoTimeToCentimeters( unsigned int echoTime );

    unsigned int ping();
    bool ping_trigger();
    unsigned int pingMedian();

};



void Radar::init()
{
    ServoDriver::init();
    ServoDriver::setPWMFreq( 60 );  // Analog servos run at ~60 Hz updates

    slew( 0 );
}




int Radar::getCurrentAngle()
{
    return mCurrentAngle;
}




int Radar::slew( int angleDegrees )
{
    // Protect against over slewing of the radar
    if ( angleDegrees > 85 )
    {
        angleDegrees = 85;
    }
    if ( angleDegrees < -85 )
    {
        angleDegrees = -85;
    }

    mCurrentAngle = angleDegrees;

    uint16_t pulseLen = convertToPulseLenFromDegreesRelative( mCurrentAngle );
    ServoDriver::setPWM( ServoDriver::kRangeSensorServoPin, 0, pulseLen );

    return mCurrentAngle;
}




uint16_t Radar::convertToPulseLenFromDegreesRelative( int8_t degrees )
{
/*
 *    -90 = 155
 *      0 = 381
 *     90 = 605
 */

    int16_t tmp = 5 * static_cast<int16_t>( degrees );
    tmp /= 2;
    tmp += 381;
    return static_cast<uint16_t>( tmp );
}





int Radar::getDistanceInCm()
{
#if 0

    pinMode( kRangeSensorDataPin, OUTPUT );
    digitalWrite( kRangeSensorDataPin, LOW );
    delayMicroseconds( 2 );
    digitalWrite( kRangeSensorDataPin, HIGH );
    delayMicroseconds( 5 );
    digitalWrite( kRangeSensorDataPin, LOW );

    pinMode( kRangeSensorDataPin, INPUT );
    long duration = pulseIn( kRangeSensorDataPin, HIGH, 20000 );

    if ( duration >= 20000 )
    {
        return 9999;
    }

    return static_cast<int>( convertMicrosecondsToCentimeters( duration ) );

#endif

    return static_cast<int>( convertEchoTimeToCentimeters( pingMedian() ) );
}






unsigned int Radar::convertEchoTimeToCentimeters( unsigned int echoTime )
{
    if ( echoTime )
    {
        float tempC = TempSensor::getTempC();

        int millisecondsPer2cm = 59;
        if ( tempC >= 17.4 )
        {
            millisecondsPer2cm = 58;
        }
        if ( tempC >= 27.3 )
        {
            millisecondsPer2cm = 57;
        }

        // Compute centimeters (with rounding).  Remember echoTime is round trip,
        // so "per2cm" factor takes care of division by 2.
        unsigned int cm = ( echoTime + millisecondsPer2cm / 2 ) / millisecondsPer2cm;

        return cm;
    }
    else
    {
        return kNoRadarEcho;
    }
}




unsigned int Radar::ping()
{
    if ( !ping_trigger() )
    {
        // Trigger a ping, if it returns false, return kNoRadarEcho to the calling function.
        return kNoRadarEcho;
    }

    unsigned long maxTime = mPingStartTime + kMaxEchoTime;

    // Wait for the ping echo.
    while ( readGpioPinDigital( pRangeSensorDataPin ) )
    {
        // Stop the loop and return kNoRadarEcho (false) if we're beyond the set maximum distance.
        if ( micros() > maxTime )
        {
            return kNoRadarEcho;
        }
    }
    return ( micros() - mPingStartTime - 5 ); // Calculate ping time, minus 5 uS of overhead.
}






bool Radar::ping_trigger()
{
    // Set trigger pin to output.
    setGpioPinModeOutput( pRangeSensorDataPin );

    // Set the trigger pin low, should already be low, but this will make sure it is.
    setGpioPinLow( pRangeSensorDataPin );

    // Wait for pin to go low, testing shows it needs 4uS to work every time.
    delayMicroseconds( 4 );

    // Set trigger pin high, this tells the sensor to send out a ping.
    setGpioPinHigh( pRangeSensorDataPin );

    // Wait for the sensor to realize the trigger pin is high.
    delayMicroseconds( kDurationOfTriggerPulse );

    // Set trigger pin back to low.
    setGpioPinLow( pRangeSensorDataPin );

    // Set trigger pin to input (because using the same Arduino pin).
    setGpioPinModeInput( pRangeSensorDataPin );

    // Set a timeout for the ping to trigger.
    unsigned long maxTime =  micros() + kMaxSensorDelay;

    // First wait for echo pin to clear.
    while ( ( readGpioPinDigital( pRangeSensorDataPin ) ) && micros() <= maxTime )
    {}

    // Wait for ping to start.
    while ( !( readGpioPinDigital( pRangeSensorDataPin ) ) )
    {
        if ( micros() > maxTime )
        {
            // Something went wrong, abort.
            return false;
        }
    }

    // Ping started, set the time.
    mPingStartTime = micros();

    // Ping started successfully.
    return true;
}



unsigned int Radar::pingMedian()
{
    const uint8_t kNbrMedianSamples = 5;

    unsigned int uS[ kNbrMedianSamples ];
    uS[ 0 ] = kNoRadarEcho;

    uint8_t it = kNbrMedianSamples;
    uint8_t i = 0;
    while ( i < it )
    {
        unsigned int last = ping();           // Send ping.
        if ( last == kNoRadarEcho )
        {
            // Ping out of range.
            // Skip, don't include as part of median.
            it--;
            // Adjust "last" variable so delay is correct length.
            last = kMaxEchoTime;
        }
        else
        {
            uint8_t j;
            // Ping in range, include as part of median.
            if ( i > 0 )
            {
                // Don't start sort till second ping.
                for ( j = i; j > 0 && uS[j - 1] < last; j-- )
                {
                    // Insertion sort loop.
                    uS[j] = uS[j - 1]; // Shift ping array to correct position for sort insertion.
                }
            }
            else
            {
                j = 0;              // First ping is starting point for sort.
            }
            uS[j] = last;              // Add last ping to array in sorted position.
            i++;                       // Move to next ping.
        }

        if ( i < it )
        {
            // Millisecond delay between pings.
            delayMilliseconds( kDelayBetweenPings );
        }
    }

    return (uS[it >> 1]); // Return the median ping distance.
}




#if 0

long convertMicrosecondsToCentimeters( long microseconds )
{
    float tempC = getTempC();

    float speedSound = 331.5 + 0.6 * tempC;                 // m/s
    float dist = microseconds * speedSound / 10000 / 2;     // cm
    return static_cast<int>( dist + 0.5 );
}

#endif


/*
    uint8_t     mServoPin;
    uint8_t     mDataPin;
*/

