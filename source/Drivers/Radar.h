#ifndef Radar_h
#define Radar_h

#include <stdint.h>
#include <limits.h>





namespace Radar
{
    
    const unsigned int kNoRadarEcho = INT_MAX;

    void init();

    int slew( int angleDegrees );

    int getCurrentAngle();

    int getDistanceInCm();


};



#endif
