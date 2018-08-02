/*
 
 A 4-element vector
 
 */

#include "Constants.h"
#include "Vector4.h"

#include <math.h>

namespace NYX {

//Constructors
Vector4::Vector4()
{
    mData[0] = 0.0f;
    mData[1] = 0.0f;
    mData[2] = 0.0f;
	mData[3] = 0.0f;
}

Vector4::Vector4(float x, float y, float z, float w)
{
    mData[0] = x;
    mData[1] = y;
    mData[2] = z;
	mData[3] = w;
}

Vector4::Vector4(float components[4])
{
    mData[0] = components[0];
    mData[1] = components[1];
    mData[2] = components[2];
	mData[3] = components[3];
}

Vector4::Vector4(const Vector4 &source)
{
    mData[0] = source.mData[0];
    mData[1] = source.mData[1];
    mData[2] = source.mData[2];
	mData[3] = source.mData[3];
}

Vector4::Vector4(const Vector3 &source3)
{
	mData[0] = source3.mData[0];
    mData[1] = source3.mData[1];
    mData[2] = source3.mData[2];
	mData[3] = 0.0;
}

Vector4::~Vector4()
{

}

//Opearators overloading
float& Vector4::operator() (const unsigned short i)
{
    //check if the requested index is out of bounds.
    if (i < 4)
        return mData[i];
    else
    {
        std::cout << "Error when calling CMatrix4x4 operator(), requested indexes are out of bounds." << std::endl;
        return mData[0];
    }
}

Vector4 Vector4::operator+ ( Vector4 vector )
{
    Vector4 out;

    out(0) = mData[0] + vector(0);
    out(1) = mData[1] + vector(1);
    out(2) = mData[2] + vector(2);
	out(3) = mData[3] + vector(3);

    return out;
}

Vector4 Vector4::operator+ (const float scalar)
{
    Vector4 out;

    out(0) = mData[0] + scalar;
    out(1) = mData[1] + scalar;
    out(2) = mData[2] + scalar;
	out(3) = mData[3] + scalar;

    return out;
}

Vector4 Vector4::operator- ( Vector4 vector )
{
    Vector4 out;

    out(0) = mData[0] - vector(0);
    out(1) = mData[1] - vector(1);
    out(2) = mData[2] - vector(2);
	out(3) = mData[3] - vector(3);

    return out;
}

Vector4 Vector4::operator- (const float scalar)
{
    Vector4 out;

    out(0) = mData[0] - scalar;
    out(1) = mData[1] - scalar;
    out(2) = mData[2] - scalar;
	out(3) = mData[3] - scalar;

    return out;
}

Vector4 Vector4::operator* ( Vector4 vector )
{
    Vector4 out;

    out(0) = mData[0]*vector(0);
    out(1) = mData[1]*vector(1);
    out(2) = mData[2]*vector(2);
	out(3) = mData[3]*vector(3);

    return out;
}

Vector4 Vector4::operator* ( const float scalar )
{
    Vector4 out;

    out(0) = mData[0]*scalar;
    out(1) = mData[1]*scalar;
    out(2) = mData[2]*scalar;
	out(3) = mData[3]*scalar;

    return out;
}

float Vector4::Dot( Vector4 vector )
{
   return ( mData[0] * vector(0) +  mData[1] * vector(1) + mData[2] * vector(2) + mData[3] * vector(3) );
}


Vector4& Vector4::operator= ( Vector4 vector )
{
    //if the same instance as "this" is passed as argument, return this without performing
    //any operation
    if (this == &vector)
        return *this;

    mData[0] = vector(0);
    mData[1] = vector(1);
    mData[2] = vector(2);
	mData[3] = vector(3);

    return *this;
}

Vector4& Vector4::operator= (Vector3 vector)
{
	mData[0] = vector(0);
    mData[1] = vector(1);
    mData[2] = vector(2);
	mData[3] = 0.0;

    return *this;
}

bool Vector4::operator== ( Vector4 vector )
{
    if ( (mData[0] == vector(0)) &&
         (mData[1] == vector(1)) &&
         (mData[2] == vector(2)) && 
		 (mData[3] == vector(3)))
        return true;
    else
        return false;
}

bool Vector4::operator!= (Vector4 vector)
{
    if ( ( mData[0] == vector.X() ) && ( mData[1] == vector.Y() ) 
		&& ( mData[2] == vector.Z() ) && ( mData[3] == vector.W() ) )
        return false;
	else
		return true;
}

//other functions
float Vector4::GetMagnitude()
{
    return sqrt( mData[0]*mData[0] + mData[1]*mData[1] + mData[2]*mData[2] + mData[3]*mData[3] );
}

Vector4 Vector4::UnitVector()
{
	Vector4 out;

	float mag = GetMagnitude();

	if (mag == 0)
		return out;

	out.X() = mData[0] / mag;
	out.Y() = mData[1] / mag;
	out.Z() = mData[2] / mag;
	out.W() = mData[3] / mag;

	return out;
}

void Vector4::SetComponents(float x, float y, float z, float w)
{
    mData[0] = x;
    mData[1] = y;
    mData[2] = z;
	mData[3] = w;
}

float* Vector4::GetComponents()
{
    return mData;
}

Vector3 Vector4::GetVector3()
{
	return Vector3(mData[0], mData[1], mData[2]);
}

float& Vector4::X()
{
    return mData[0];
}

float& Vector4::Y()
{
    return mData[1];
}

float& Vector4::Z()
{
    return mData[2];
}

float& Vector4::W()
{
    return mData[3];
}

std::ostream & operator << (std::ostream &out, Vector4 &vec)
{
    out << vec.X() << " " << vec.Y() << " " << vec.Z() << " " << vec.W();
    return out;
}    
    
}