/*

A 4-element vector

*/
#ifndef VECTOR4_H
#define VECTOR4_H

#include "vector3.h"
#include <iostream>

namespace NYX
{

//! \brief 3D vector. simplifies most of the operations in the physics and graphics systems
class NYX_EXPORT Vector4
{
public:

    Vector4(); //! empty constructor
    Vector4(float x, float y, float z, float w = 1.0); //! constructor. vector components are initialised with x, y, z
    Vector4(float components[4]); //! constructor. takes a pointer to float. nice to have when initialising vectors from rows of CMatrix4x4
    Vector4(const Vector4 &source); //! copy constructor
	Vector4(const Vector3 &source3); //! copy constructor
    ~Vector4(); //! deconstructor

    float& operator() (const unsigned short i);//! outputs mData[i]. i must be between 0 and 2
    Vector4 operator+ (Vector4 vector); //! sum of 2 vectors
    Vector4 operator+ (const float scalar); //! sum of vector and scalar constant
    Vector4 operator- (Vector4 vector); //! subtraction of 2 vectors
    Vector4 operator- (const float scalar); //! subtraction of scalar constant
    Vector4 operator* (Vector4 vector); //! element-wise multiplication
    Vector4 operator* (const float scalar); //! multiplication by a scalar value
    float Dot(Vector4 vector); //! scalar (dot) product.
    
    Vector4& operator= (Vector4 vector); //! assignement
	Vector4& operator= (Vector3 vector); //! assignement
    bool operator== (Vector4 vector);//! equality test
    bool operator!= (Vector4 vector); //! not equal test

    float GetMagnitude(); //! returns the vector magnitude
	Vector4 UnitVector(); //! returns the unit vector of the current vector
    void SetComponents(float x, float y, float z, float w = 1.0); //! sets (or re-sets) the vector components to x, y, z
    float* GetComponents(); //! provides in output the pointer to mData
	Vector3 GetVector3();
    float& X(); //! returns mData[0]
    float& Y(); //! returns mData[1]
    float& Z(); //! returns mData[2]
	float& W(); //! returns mData[3]

    friend std::ostream & operator << (std::ostream &out, Vector4 &vec);

private:

    float mData[4];
};

inline Vector4 ComputePlaneEq(Vector3 p1, Vector3 p2, Vector3 p3)
{
	Vector3 v1, v2, normal;
	float tmp;

	v1 = p2 - p1;
	v2 = p3 - p1;
	normal = v1.Cross(v2);
	normal = normal.UnitVector();
	tmp = -(normal(0) * p3(0) + normal(1) * p3(1) + normal(2) * p3(2));

	return Vector4(normal(0), normal(1), normal(2), tmp);
}

inline Vector4 ComputePlaneEq(Vector4 p1, Vector4 p2, Vector4 p3)
{
	Vector3 pp1 = p1.GetVector3();
	Vector3 pp2 = p2.GetVector3();
	Vector3 pp3 = p3.GetVector3();
 	Vector3 v1, v2, normal;
	float tmp;

	v1 = pp2 - pp1;
	v2 = pp3 - pp1;
	normal = v1.Cross(v2);
	normal = normal.UnitVector();
	tmp = -(normal(0) * pp3(0) + normal(1) * pp3(1) + normal(2) * pp3(2));

	return Vector4(normal(0), normal(1), normal(2), tmp);
}

inline float ComputeDistanceToPlane(Vector3 point, Vector4 plane)
{
	return (point(0)*plane(0) + point(1)*plane(1) + point(2)*plane(2) + plane(3));
}

}

#endif // VECTOR4_H
