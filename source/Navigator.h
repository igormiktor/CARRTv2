/*************************************************
 *
 * Navigator -- an inertial navigation class
 *
 *************************************************/


#ifndef Navigator_h
#define Navigator_h


#include "VectorUtils.h"



class Navigator
{
public:

    Navigator();

    void init();

    void doNavUpdate();

    void doDriftCorrection();

    float getCurrentHeading()
    { return mCurrentHeading; }

    Vector2Float getCurrentPosition()
    { return mCurrentPosition; }

    Vector2Float getCurrentSpeed()
    { return mCurrentSpeed; }

    Vector2Float getCurrentAcceleration()
    { return mCurrentAcceleration; }

    Vector3Int getRestStateAcceleration()
    { return mAccelerationZero; }

    Vector3Int getRestStateAngularRate()
    { return mGyroZero; }

    void moving()
    { moving( kComboMove ); }

    void movingStraight()
    { moving( kStraightMove ); }

    void movingTurning()
    { moving( kTurnMove ); }

    bool isMoving()
    { return mMoving; }

    void stopped();

    void reset();

    void hardReset();


private:

    enum Motion { kStopped = 0, kStraightMove = 0x01, kTurnMove = 0x10, kComboMove = 0x11 };

    void moving( Motion kindOfMove );

    void updateOrientation( Vector3Float g, Vector3Float a, Vector3Float m );
    void updateIntegration( const Vector2Float& newAcceleration, Vector2Float* newSpeed, Vector2Float* newPosition );

    float limitSpeed( float v );
    float limitRotationRate( float r );

    Vector2Float filterAndConvertAccelerationDataToMetersPerSec2( const Vector3Int& in );
    float filterAndConvertGyroscopeDataToZDegreesPerSec( const Vector3Int& in );
    void determineNewHeading( float magHeadingChange, float gyroHeadingChange );


    Vector3Int      mAccelerationZero;
    Vector3Int      mGyroZero;

    Vector2Float    mCurrentAcceleration;
    Vector2Float    mCurrentSpeed;
    Vector2Float    mCurrentPosition;

    float           mCurrentHeading;
    float           mAccumulatedCompassDrift;
    int             mAccumulationCount;

    Motion          mMoving;

};



#endif
