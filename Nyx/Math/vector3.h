/*

A 3-element vector

*/
#ifndef VECTOR3_H
#define VECTOR3_H

//#include "vector4.h"
#include <iostream>

namespace NYX
{

class Vector4;

//! \brief 3D vector. simplifies most of the operations in the physics and graphics systems
class NYX_EXPORT Vector3
{
	friend class Vector4;

public:

    Vector3(); //! empty constructor
    Vector3(float x, float y, float z); //! constructor. vector components are initialised with x, y, z
    Vector3(float components[3]); //! constructor. takes a pointer to float. nice to have when initialising vectors from rows of CMatrix4x4
    Vector3(const Vector3 &source); //! copy constructor
    ~Vector3(); //! deconstructor

    float& operator() (const unsigned short i);//! outputs mData[i]. i must be between 0 and 2
    Vector3 operator+ (Vector3 vector); //! sum of 2 vectors
    Vector3 operator+ (const float scalar); //! sum of vector and scalar constant
    Vector3 operator- (Vector3 vector); //! subtraction of 2 vectors
    Vector3 operator- (const float scalar); //! subtraction of scalar constant
    Vector3 operator* (Vector3 vector); //! element-wise multiplication
    Vector3 operator* (const float scalar); //! multiplication by a scalar value
    float Dot(Vector3 vector); //! scalar (dot) product.
    Vector3 Cross(Vector3 vector); //! vector (cross) product
    Vector3& operator= (Vector3 vector); //! assignement
    bool operator== (Vector3 vector);//! equality test
    bool operator!= (Vector3 vector); //! not equal test

    float GetMagnitude(); //! returns the vector magnitude
	Vector3 UnitVector(); //! returns the unit vector of the current vector
    void SetComponents(float x, float y, float z); //! sets (or re-sets) the vector components to x, y, z
    float* GetComponents(); //! provides in output the pointer to mData
    float& X(); //! returns mData[0]
    float& Y(); //! returns mData[1]
    float& Z(); //! returns mData[2]

    static Vector3 RotateX(float angle, Vector3 vecIn); //! returns a vector equivalent to RotX * vecIn
    static Vector3 RotateY(float angle, Vector3 vecIn); //! returns a vector equivalent to RotY * vecIn
    static Vector3 RotateZ(float angle, Vector3 vecIn); //! returns a vector equivalent to RotX * vecIn

    friend std::ostream & operator << (std::ostream &out, Vector3 &vec);
    
private:

    float mData[3];
};

}

#endif // VECTOR3_H
