/*

Quaternion Implementation

*/
#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector3.h"

#include <iostream>

namespace NYX
{

//! \brief Quaternion
class NYX_EXPORT Quaternion
{
public:

    Quaternion(); //! empty constructor
    Quaternion(float a, float x, float y, float z); //! constructor. vector components are initialised with x, y, z
    Quaternion(float components[4]); //! constructor. takes a pointer to float. nice to have when initialising vectors from rows of CMatrix4x4
    Quaternion(float scalar, Vector3 vector, bool isRotation); //! constructor, takes a scalar part and a vector part to initialize the quaternion, or a constructs a quaternion rotation, angle in radians 
	Quaternion(const Quaternion &source); //! copy constructor

    ~Quaternion(); //! deconstructor

    float& operator() (const unsigned short i);//! outputs mData[i]. i must be between 0 and 2
    Quaternion operator+ (Quaternion quat); //! sum of 2 quaternions
    Quaternion operator+ (const float scalar); //! sum of a quaternion and a scalar constant
    Quaternion operator- (Quaternion quat); //! subtraction of 2 quaternions
    Quaternion operator- (const float scalar); //! subtraction of scalar constant
    Quaternion operator* (Quaternion quat); //! quaternion multiplication
	Quaternion operator* (Vector3 vector); //! quaternion x vector multiplication
    Quaternion operator* (const float scalar); //! multiplication by a scalar value
    Quaternion& operator= (Quaternion quat); //! assignement
    bool operator== (Quaternion quat);//! equality test
    bool operator!= (Quaternion quat); //! inequality test

    float Norm(); //! returns the quaternion norm
	void Normalize(); //! tranforms to unit quaternion
    void SetComponents(float a, float x, float y, float z); //! sets (or re-sets) the quaternion components to a, x, y, z
	void SetComponents(float a, Vector3 vector); //! sets (or re-sets) the quaternion components to a, vector.x, vector.y, vector.z
    void SetRotationComponents(float angle, Vector3 rotationAxis); //! constructs a quaternion rotation, angle in radians
	float* GetComponents(); //! provides in output the pointer to mData
    
	float& GetScalarPart(); //! returns mData[0]
    Vector3 GetVectorPart(); //! returns CVector3(mData[1], mData[2], mData[3])
	Quaternion Conjugate(); //! returns CQuaternion(mData[0], -mData[1], -mData[2], -mData[3])
	Vector3 RotateVector(Vector3 vector); //! performs a vector rotation

	friend std::ostream & operator << (std::ostream &out, Quaternion &quat);

	// TODO 
	// 1) return angle and unit vector from quaternion
	// 2) return rotation matrix from quaternion
	// 3) (optional) compute quaternion for euler rotations. Note: requires an enumeration for the possible sequences of euler rotations.

private:

    float mData[4];
};

}

#endif //QUATERNION_H
