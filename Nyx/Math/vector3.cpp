/*

 A 3-element vector
 
 */

#include "Constants.h"
#include "Vector3.h"

#include <math.h>

namespace NYX {

//Constructors
Vector3::Vector3()
{
    mData[0] = 0.0f;
    mData[1] = 0.0f;
    mData[2] = 0.0f;
}

Vector3::Vector3(float x, float y, float z)
{
    mData[0] = x;
    mData[1] = y;
    mData[2] = z;
}

Vector3::Vector3(float components[3])
{
    mData[0] = components[0];
    mData[1] = components[1];
    mData[2] = components[2];
}

Vector3::Vector3(const Vector3 &source)
{
    mData[0] = source.mData[0];
    mData[1] = source.mData[1];
    mData[2] = source.mData[2];
}

Vector3::~Vector3()
{

}

//Opearators overloading
float& Vector3::operator() (const unsigned short i)
{
    //check if the requested index is out of bounds.
    if (i < 3)
        return mData[i];
    else
    {
        std::cout << "Error when calling CMatrix4x4 operator(), requested indexes are out of bounds." << std::endl;
        return mData[0];
    }
}

Vector3 Vector3::operator+ ( Vector3 vector )
{
    Vector3 out;

    out(0) = mData[0] + vector(0);
    out(1) = mData[1] + vector(1);
    out(2) = mData[2] + vector(2);

    return out;
}

Vector3 Vector3::operator+ (const float scalar)
{
    Vector3 out;

    out(0) = mData[0] + scalar;
    out(1) = mData[1] + scalar;
    out(2) = mData[2] + scalar;

    return out;
}

Vector3 Vector3::operator- ( Vector3 vector )
{
    Vector3 out;

    out(0) = mData[0] - vector(0);
    out(1) = mData[1] - vector(1);
    out(2) = mData[2] - vector(2);

    return out;
}

Vector3 Vector3::operator- (const float scalar)
{
    Vector3 out;

    out(0) = mData[0] - scalar;
    out(1) = mData[1] - scalar;
    out(2) = mData[2] - scalar;

    return out;
}

Vector3 Vector3::operator* ( Vector3 vector )
{
    Vector3 out;

    out(0) = mData[0]*vector(0);
    out(1) = mData[1]*vector(1);
    out(2) = mData[2]*vector(2);

    return out;
}

Vector3 Vector3::operator* ( const float scalar )
{
    Vector3 out;

    out(0) = mData[0]*scalar;
    out(1) = mData[1]*scalar;
    out(2) = mData[2]*scalar;

    return out;
}

float Vector3::Dot( Vector3 vector )
{
   return ( mData[0] * vector(0) +  mData[1] * vector(1) + mData[2] * vector(2) );
}

Vector3 Vector3::Cross( Vector3 vector )
{
    Vector3 out;

    out(0) = mData[1]*vector(2) - mData[2]*vector(1);
    out(1) = mData[2]*vector(0) - mData[0]*vector(2);
    out(2) = mData[0]*vector(1) - mData[1]*vector(0);

    return out;
}

Vector3& Vector3::operator= ( Vector3 vector )
{
    //if the same instance as "this" is passed as argument, return this without performing
    //any operation
    if (this == &vector)
        return *this;

    mData[0] = vector(0);
    mData[1] = vector(1);
    mData[2] = vector(2);

    return *this;
}

bool Vector3::operator== ( Vector3 vector )
{
    if ( (mData[0] == vector(0)) &&
         (mData[1] == vector(1)) &&
         (mData[2] == vector(2)) )
        return true;
    else
        return false;
}

bool Vector3::operator!= (Vector3 vector)
{
    bool ret = false;

    if ( ( mData[0] == vector.X() ) && ( mData[1] == vector.Y() ) && ( mData[2] == vector.Z() ) )
        ret = true;

    return ret;
}

//other functions
float Vector3::GetMagnitude()
{
    return sqrt( mData[0]*mData[0] + mData[1]*mData[1] + mData[2]*mData[2] );
}

Vector3 Vector3::UnitVector()
{
	Vector3 out;

	float mag = GetMagnitude();

	if (mag == 0)
		return out;

	out.X() = mData[0] / mag;
	out.Y() = mData[1] / mag;
	out.Z() = mData[2] / mag;

	return out;
}

void Vector3::SetComponents(float x, float y, float z)
{
    mData[0] = x;
    mData[1] = y;
    mData[2] = z;
}

float* Vector3::GetComponents()
{
    return mData;
}

float& Vector3::X()
{
    return mData[0];
}

float& Vector3::Y()
{
    return mData[1];
}

float& Vector3::Z()
{
    return mData[2];
}

Vector3 Vector3::RotateX(float angle, Vector3 vecIn)
{
    Vector3 out;

	float sAngle, cAngle;

	sAngle = sin(angle);
	cAngle = cos(angle);

    out.SetComponents( vecIn.X(),
                       vecIn.Y() * cAngle - vecIn.Z() * sAngle,
                       vecIn.Y() * sAngle + vecIn.Z() * cAngle );

    return out;
}

Vector3 Vector3::RotateY(float angle, Vector3 vecIn)
{
    Vector3 out;

	float sAngle, cAngle;

	sAngle = sin(angle);
	cAngle = cos(angle);

    out.SetComponents( vecIn.X() * cAngle + vecIn.Z() * sAngle,
                       vecIn.Y(),
                       -vecIn.X() * sAngle + vecIn.Z() * cAngle );

    return out;
}

Vector3 Vector3::RotateZ(float angle, Vector3 vecIn)
{
    Vector3 out;

	float sAngle, cAngle;

	sAngle = sin(angle);
	cAngle = cos(angle);

    out.SetComponents( vecIn.X() * cAngle - vecIn.Y() * sAngle,
                       vecIn.X() * sAngle + vecIn.Y() * cAngle,
                       vecIn.Z() );

    return out;
}

std::ostream & operator << (std::ostream &out, Vector3 &vec)
{
    out << vec.X() << " " << vec.Y() << " " << vec.Z();
    return out;
}    
    
}