/*

Quaternion Implementation

*/

#include "Constants.h"
#include "Quaternion.h"

#include <math.h>

namespace NYX {

//Constructors
Quaternion::Quaternion()
{
    mData[0] = 0.0f;
    mData[1] = 0.0f;
    mData[2] = 0.0f;
	mData[3] = 0.0f;
}

Quaternion::Quaternion(float a, float x, float y, float z)
{
    mData[0] = a;
    mData[1] = x;
    mData[2] = y;
	mData[3] = z;
}

Quaternion::Quaternion(float components[4])
{
    mData[0] = components[0];
    mData[1] = components[1];
    mData[2] = components[2];
	mData[3] = components[3];
}

Quaternion::Quaternion(float scalar, Vector3 vector, bool isRotation)
{
	if (!isRotation)
	{
		mData[0] = scalar;
		mData[1] = vector.X();
		mData[2] = vector.Y();
		mData[3] = vector.Z();
	}
	else
	{
		//in this case scalar is an angle in radians
		mData[0] = cos( scalar / float(2.0) );

		float sinHalfAngle = sin( scalar / float(2.0) );

		// make sure rotation vector is a unit vector
		Vector3 rotV = vector.UnitVector();

		mData[1] = rotV.X() * sinHalfAngle;
		mData[2] = rotV.Y() * sinHalfAngle;
		mData[3] = rotV.Z() * sinHalfAngle;
	}
}

Quaternion::Quaternion(const Quaternion &source)
{
    mData[0] = source.mData[0];
    mData[1] = source.mData[1];
    mData[2] = source.mData[2];
	mData[3] = source.mData[3];
}

Quaternion::~Quaternion()
{

}

//Opearators overloading
float& Quaternion::operator() (const unsigned short i)
{
    //check if the requested index is out of bounds.
    if (i < 4)
        return mData[i];
    else
    {
        std::cout << "Error when calling Quaternion operator(), requested indexes are out of bounds." << std::endl;
        return mData[0];
    }
}

Quaternion Quaternion::operator+ ( Quaternion quat )
{
    Quaternion out;

    out(0) = mData[0] + quat(0);
    out(1) = mData[1] + quat(1);
    out(2) = mData[2] + quat(2);
	out(3) = mData[3] + quat(3);
	
    return out;
}

Quaternion Quaternion::operator+ (const float scalar)
{
    Quaternion out;

    out(0) = mData[0] + scalar;
    out(1) = mData[1];
    out(2) = mData[2];
	out(3) = mData[3];

    return out;
}

Quaternion Quaternion::operator- ( Quaternion quat )
{
    Quaternion out;

    out(0) = mData[0] - quat(0);
    out(1) = mData[1] - quat(1);
    out(2) = mData[2] - quat(2);
	out(3) = mData[3] - quat(3);

    return out; 
}

Quaternion Quaternion::operator- (const float scalar)
{
    Quaternion out;

    out(0) = mData[0] - scalar;
    out(1) = mData[1];
    out(2) = mData[2];
	out(3) = mData[3];

    return out; 
}

Quaternion Quaternion::operator* ( Quaternion quat )
{
    Quaternion out;

	out(0) = mData[0]*quat(0) - mData[1]*quat(1) - mData[2]*quat(2) - mData[3]*quat(3);
	out(1) = mData[0]*quat(1) + mData[1]*quat(0) + mData[2]*quat(3) - mData[3]*quat(2);
	out(2) = mData[0]*quat(2) - mData[1]*quat(3) + mData[2]*quat(0) + mData[3]*quat(1);
	out(3) = mData[0]*quat(3) + mData[1]*quat(2) - mData[2]*quat(1) + mData[3]*quat(0);

    return out;
}

Quaternion Quaternion::operator* (Vector3 vector)
{
	Quaternion out;

	out(0) = - mData[1]*vector(0) - mData[2]*vector(1) - mData[3]*vector(2);
	out(1) = mData[0]*vector(0) + mData[2]*vector(2) - mData[3]*vector(1);
	out(2) = mData[0]*vector(1) - mData[1]*vector(2) + mData[3]*vector(0);
	out(3) = mData[0]*vector(2) + mData[1]*vector(1) - mData[2]*vector(0);

    return out;
}

Quaternion Quaternion::operator* ( const float scalar )
{
    Quaternion out;

	out(0) = mData[0]*scalar;
	out(1) = mData[1]*scalar;
	out(2) = mData[2]*scalar;
	out(3) = mData[3]*scalar;

    return out; 
}

Quaternion& Quaternion::operator= ( Quaternion quat )
{
    //if the same instance as "this" is passed as argument, return this without performing
    //any operation
    if (this == &quat)
        return *this;

    mData[0] = quat(0);
    mData[1] = quat(1);
    mData[2] = quat(2);
	mData[3] = quat(3);

    return *this;
}

bool Quaternion::operator== ( Quaternion quat )
{
    if ( (mData[0] == quat(0)) &&
         (mData[1] == quat(1)) &&
         (mData[2] == quat(2)) && 
		 (mData[3] == quat(3)) )
        return true;
    else
        return false;
}

bool Quaternion::operator!= (Quaternion quat)
{
    bool ret = false;

    if ( ( mData[0] == quat(0) ) && ( mData[1] == quat(1) ) && ( mData[2] == quat(2) ) 
		&& ( mData[3] == quat(3) ))
        ret = true;

    return ret;
}

//other functions
float Quaternion::Norm()
{
    return sqrt( mData[0]*mData[0] + mData[1]*mData[1] + mData[2]*mData[2] + mData[3]*mData[3] );
}

void Quaternion::Normalize()
{
	mData[0] /= Norm();
	mData[1] /= Norm();
	mData[2] /= Norm();
	mData[3] /= Norm();
}

void Quaternion::SetComponents(float a, float x, float y, float z)
{
	mData[0] = a;
    mData[1] = x;
    mData[2] = y;
    mData[3] = z;
}

void Quaternion::SetComponents(float a, Vector3 vector)
{
	mData[0] = a;
	mData[1] = vector.X();
    mData[2] = vector.Y();
	mData[3] = vector.Z();
}

void Quaternion::SetRotationComponents(float angle, Vector3 rotationAxis)
{
	mData[0] = cos( angle / float(2.0) );

	float sinHalfAngle = sin( angle / float(2.0) );

	// make sure rotation vector is a unit vector
	Vector3 rotV = rotationAxis.UnitVector();

	mData[1] = rotV.X() * sinHalfAngle;
	mData[2] = rotV.Y() * sinHalfAngle;
	mData[3] = rotV.Z() * sinHalfAngle;
}

float* Quaternion::GetComponents()
{
    return mData;
}

float& Quaternion::GetScalarPart()
{
	return mData[0];
}

Vector3 Quaternion::GetVectorPart()
{
	return Vector3(mData[1], mData[2], mData[3]);
}

Quaternion Quaternion::Conjugate()
{
	return Quaternion(mData[0], -mData[1], -mData[2], -mData[3]);
}

Vector3 Quaternion::RotateVector(Vector3 vector)
{
	Quaternion tempQ;
	Quaternion conjQ = Conjugate();

	tempQ = (*this)*vector*conjQ;

	return tempQ.GetVectorPart();
}

std::ostream & operator<< (std::ostream &out, Quaternion &quat)
{
	Vector3 vec = quat.GetVectorPart();

	out << quat.GetScalarPart() << " " << vec.X() << " " << vec.Y() << " " << vec.Z();
	return out;
}

}
