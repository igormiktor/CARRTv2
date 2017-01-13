/*
    VectorUtils.h - Various vector definitions and utilities

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




#ifndef VectorUtils_h
#define VectorUtils_h

#include <inttypes.h>
#include <math.h>



struct Vector3Int
{
    int16_t    x;
    int16_t    y;
    int16_t    z;

    Vector3Int() {}
    Vector3Int( int xx, int yy, int zz )
    : x( xx ), y( yy ), z( zz ) {}

    Vector3Int& operator+=( const Vector3Int& rhs )
    { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

    Vector3Int& operator-=( const Vector3Int& rhs )
    { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    Vector3Int& operator*=( float rhs )
    { x *= rhs; y *= rhs; z *= rhs; return *this; }

    Vector3Int& operator/=( int16_t rhs )
    { x /= rhs; y /= rhs; z /= rhs; return *this; }

    Vector3Int& operator>>=( uint8_t rhs )
    { x >>= rhs; y >>= rhs; z >>= rhs; return *this; }
};


inline Vector3Int operator+( const Vector3Int& a, const Vector3Int& b )
{
    Vector3Int tmp( a ); tmp += b; return tmp;
}


inline Vector3Int operator-( const Vector3Int& a, const Vector3Int& b )
{
    Vector3Int tmp( a ); tmp -= b; return tmp;
}


inline Vector3Int operator>>( const Vector3Int& a, uint8_t b )
{
    Vector3Int tmp( a ); tmp >>= b; return tmp;
}


inline Vector3Int operator*( const Vector3Int& a, int16_t b )
{
    Vector3Int tmp( a ); tmp *= b; return tmp;
}


inline Vector3Int operator*( int16_t a, const Vector3Int& b )
{
    Vector3Int tmp( b ); tmp *= a; return tmp;
}


inline int16_t operator*( const Vector3Int& a, const Vector3Int& b )
{
    return a.x * b.x + a.y * b.y+ a.z * b.z;
}


inline float norm( const Vector3Int& a )
{
    return sqrt( a * a );
}





struct Vector3Long
{
    int32_t    x;
    int32_t    y;
    int32_t    z;

    Vector3Long() {}
    Vector3Long( int32_t xx, int32_t yy, int32_t zz )
    : x( xx ), y( yy ), z( zz ) {}

    // Allow implicit conversions from int16_t vectors to int32_t vectors
    // cppcheck-suppress noExplicitConstructor
    Vector3Long( const Vector3Int& v )
    : x( v.x ), y( v.y ), z( v.z ) {}

    const Vector3Long& operator=( const Vector3Int& v )
    { x = v.x; y = v.y; z = v.z; return *this; }

    Vector3Long& operator+=( const Vector3Long& rhs )
    { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

    Vector3Long& operator-=( const Vector3Long& rhs )
    { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    Vector3Long& operator+=( const Vector3Int& rhs )
    { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

    Vector3Long& operator-=( const Vector3Int& rhs )
    { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    Vector3Long& operator*=( float rhs )
    { x *= rhs; y *= rhs; z *= rhs; return *this; }

    Vector3Long& operator/=( int32_t rhs )
    { x /= rhs; y /= rhs; z /= rhs; return *this; }

    Vector3Long& operator>>=( uint8_t rhs )
    { x >>= rhs; y >>= rhs; z >>= rhs; return *this; }
};


inline Vector3Long operator+( const Vector3Long& a, const Vector3Long& b )
{
    Vector3Long tmp( a ); tmp += b; return tmp;
}


inline Vector3Long operator-( const Vector3Long& a, const Vector3Long& b )
{
    Vector3Long tmp( a ); tmp -= b; return tmp;
}


inline Vector3Long operator>>( const Vector3Long& a, uint8_t b )
{
    Vector3Long tmp( a ); tmp >>= b; return tmp;
}


inline Vector3Long operator*( const Vector3Long& a, int32_t b )
{
    Vector3Long tmp( a ); tmp *= b; return tmp;
}


inline Vector3Long operator*( int32_t a, const Vector3Long& b )
{
    Vector3Long tmp( b ); tmp *= a; return tmp;
}


inline int32_t operator*( const Vector3Long& a, const Vector3Long& b )
{
    return a.x * b.x + a.y * b.y+ a.z * b.z;
}


inline float norm( const Vector3Long& a )
{
    return sqrt( a * a );
}





struct Vector3Float
{
    float       x;
    float       y;
    float       z;

    Vector3Float() {}
    Vector3Float( float xx, float yy, float zz )
    : x( xx ), y( yy ), z( zz ) {}

    // Allow implicit conversions from integer vectors to float vectors
    // cppcheck-suppress noExplicitConstructor
    Vector3Float( const Vector3Int& v )
    : x( v.x ), y( v.y ), z( v.z ) {}

    const Vector3Float& operator=( const Vector3Int& v )
    { x = v.x; y = v.y; z = v.z; return *this; }

    // Allow implicit conversions from long vectors to float vectors
    // cppcheck-suppress noExplicitConstructor
    Vector3Float( const Vector3Long& v )
    : x( v.x ), y( v.y ), z( v.z ) {}

    const Vector3Float& operator=( const Vector3Long& v )
    { x = v.x; y = v.y; z = v.z; return *this; }


    // Operator assignments
    Vector3Float& operator+=( const Vector3Float& rhs )
    { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

    Vector3Float& operator-=( const Vector3Float& rhs )
    { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    Vector3Float& operator+=( const Vector3Long& rhs )
    { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

    Vector3Float& operator-=( const Vector3Long& rhs )
    { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    Vector3Float& operator+=( const Vector3Int& rhs )
    { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

    Vector3Float& operator-=( const Vector3Int& rhs )
    { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    Vector3Float& operator*=( float rhs )
    { x *= rhs; y *= rhs; z *= rhs; return *this; }

    Vector3Float& operator/=( float rhs )
    { x /= rhs; y /= rhs; z /= rhs; return *this; }


    void normalize()
    { float tmp = sqrt( x*x + y*y + z*z ); x /= tmp; y /= tmp; z /= tmp; }
};


inline Vector3Float operator+( const Vector3Float& a, const Vector3Float& b )
{
    Vector3Float tmp( a ); tmp += b; return tmp;
}


inline Vector3Float operator-( const Vector3Float& a, const Vector3Float& b )
{
    Vector3Float tmp( a ); tmp -= b; return tmp;
}


inline Vector3Float operator*( const Vector3Float& a, float b )
{
    Vector3Float tmp( a ); tmp *= b; return tmp;
}


inline Vector3Float operator*( float a, const Vector3Float& b )
{
    Vector3Float tmp( b ); tmp *= a; return tmp;
}


inline Vector3Float operator/( const Vector3Float& a, float b )
{
    Vector3Float tmp( a ); tmp /= b; return tmp;
}


inline float operator*( const Vector3Float& a, const Vector3Float& b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


inline Vector3Float elementWiseMult( const Vector3Float& a, const Vector3Float& b )
{
    return Vector3Float( a.x * b.x, a.y * b.y, a.z * b.z );
}


inline float norm( const Vector3Float& a )
{
    return sqrt( a * a );
}



inline Vector3Float cross( const Vector3Float& a, const Vector3Float& b )
{
    return Vector3Float( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
}





struct Vector2Float
{
    float       x;
    float       y;

    Vector2Float() {}
    Vector2Float( float xx, float yy )
    : x( xx ), y( yy ) {}

    // Allow implicit conversions from 3D to 2D float vectors
    // cppcheck-suppress noExplicitConstructor
    Vector2Float( const Vector3Float& v )
    : x( v.x ), y( v.y ) {}

    const Vector2Float& operator=( const Vector3Float& v )
    { x = v.x; y = v.y; return *this; }


    // Operator assignments
    Vector2Float& operator+=( const Vector2Float& rhs )
    { x += rhs.x; y += rhs.y; return *this; }

    Vector2Float& operator-=( const Vector2Float& rhs )
    { x -= rhs.x; y -= rhs.y; return *this; }

    Vector2Float& operator*=( float rhs )
    { x *= rhs; y *= rhs; return *this; }

    Vector2Float& operator/=( float rhs )
    { x /= rhs; y /= rhs; return *this; }


    void normalize()
    { float tmp = sqrt( x*x + y*y ); x /= tmp; y /= tmp; }

};


inline Vector2Float operator+( const Vector2Float& a, const Vector2Float& b )
{
    Vector2Float tmp( a ); tmp += b; return tmp;
}


inline Vector2Float operator-( const Vector2Float& a, const Vector2Float& b )
{
    Vector2Float tmp( a ); tmp -= b; return tmp;
}


inline Vector2Float operator*( const Vector2Float& a, float b )
{
    Vector2Float tmp( a ); tmp *= b; return tmp;
}


inline Vector2Float operator*( float a, const Vector2Float& b )
{
    Vector2Float tmp( b ); tmp *= a; return tmp;
}


inline Vector2Float operator/( const Vector2Float& a, float b )
{
    Vector2Float tmp( a ); tmp /= b; return tmp;
}


inline float operator*( const Vector2Float& a, const Vector2Float& b )
{
    return a.x * b.x + a.y * b.y ;
}


inline float norm( const Vector2Float& a )
{
    return sqrt( a * a );
}







#endif


