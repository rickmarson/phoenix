/*
 
 A 2-element vector
 
 */
#include "Constants.h"
#include "Vector2.h"

#include <math.h>

namespace NYX {

//Constructors
Vector2::Vector2()
{
    mData[0] = 0.0f;
    mData[1] = 0.0f;
}

Vector2::Vector2(float x, float y)
{
    mData[0] = x;
    mData[1] = y;
}

Vector2::Vector2(float components[2])
{
    mData[0] = components[0];
    mData[1] = components[1];
}

Vector2::Vector2(const Vector2 &source)
{
    mData[0] = source.mData[0];
    mData[1] = source.mData[1];
}

Vector2::~Vector2()
{

}

//Opearators overloading
float& Vector2::operator() (const unsigned short i)
{
    //check if the requested index is out of bounds.
    if (i < 2)
        return mData[i];
    else
    {
        std::cout << "Error when calling CMatrix4x4 operator(), requested indexes are out of bounds." << std::endl;
        return mData[0];
    }
}

Vector2 Vector2::operator+ ( Vector2 vector )
{
    Vector2 out;

    out(0) = mData[0] + vector(0);
    out(1) = mData[1] + vector(1);

    return out;
}

Vector2 Vector2::operator+ (const float scalar)
{
    Vector2 out;

    out(0) = mData[0] + scalar;
    out(1) = mData[1] + scalar;

    return out;
}

Vector2 Vector2::operator- ( Vector2 vector )
{
    Vector2 out;

    out(0) = mData[0] - vector(0);
    out(1) = mData[1] - vector(1);

    return out;
}

Vector2 Vector2::operator- (const float scalar)
{
    Vector2 out;

    out(0) = mData[0] - scalar;
    out(1) = mData[1] - scalar;

    return out;
}

Vector2 Vector2::operator* ( Vector2 vector )
{
    Vector2 out;

    out(0) = mData[0]*vector(0);
    out(1) = mData[1]*vector(1);

    return out;
}

Vector2 Vector2::operator* ( const float scalar )
{
    Vector2 out;

    out(0) = mData[0]*scalar;
    out(1) = mData[1]*scalar;

    return out;
}

float Vector2::dot( Vector2 vector )
{
   return ( mData[0] * vector(0) +  mData[1] * vector(1) );
}

Vector2 Vector2::cross( Vector2 vector )
{
    Vector2 out;

    out(0) = mData[1]*vector(2) - mData[2]*vector(1);
    out(1) = mData[2]*vector(0) - mData[0]*vector(2);

    return out;
}

Vector2& Vector2::operator= ( Vector2 vector )
{
    //if the same instance as "this" is passed as argument, return this without performing
    //any operation
    if (this == &vector)
        return *this;

    mData[0] = vector(0);
    mData[1] = vector(1);

    return *this;
}

bool Vector2::operator== ( Vector2 vector )
{
    if ( (mData[0] == vector(0)) &&
         (mData[1] == vector(1)) )
        return true;
    else
        return false;
}

bool Vector2::operator!= (Vector2 vector)
{
    bool ret = false;

    if ( ( mData[0] == vector.X() ) && ( mData[1] == vector.Y() ) )
        ret = true;

    return ret;
}

//other functions
float Vector2::GetMagnitude()
{
    return sqrt( mData[0]*mData[0] + mData[1]*mData[1] );
}

Vector2 Vector2::UnitVector()
{
	Vector2 out;

	float mag = GetMagnitude();

	out.X() = mData[0] / mag;
	out.Y() = mData[1] / mag;

	return out;
}

void Vector2::SetComponents(float x, float y)
{
    mData[0] = x;
    mData[1] = y;
}

float* Vector2::GetComponents()
{
    return mData;
}

float& Vector2::X()
{
    return mData[0];
}

float& Vector2::Y()
{
    return mData[1];
}

Vector2 Vector2::Rotate(float angle, Vector2 vecIn)
{
    Vector2 out;

	float sAngle, cAngle;

	sAngle = sin(angle);
	cAngle = cos(angle);

    out.SetComponents( vecIn.X() * cAngle - vecIn.Y() * sAngle,
                       vecIn.X() * sAngle + vecIn.Y() * cAngle );

    return out;
}

std::ostream & operator << (std::ostream &out, Vector2 &vec)
{
    out << vec.X() << " " << vec.Y();
    return out;
}
    
}