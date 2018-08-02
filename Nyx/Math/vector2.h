/*

A 2-element vector

*/
#ifndef VECTOR2_H
#define VECTOR2_H

#include <iostream>

namespace NYX
{

//! \brief 2D vector. Mainly intended to represent pixels or GUI elements.
class NYX_EXPORT Vector2
{
public:

    Vector2(); //! empty constructor
    Vector2(float x, float y); //! constructor. vector components are initialised with x, y, z
    Vector2(float components[2]); //! constructor. takes a pointer to float. nice to have when initialising vectors from rows of CMatrix4x4
    Vector2(const Vector2 &source); //! copy constructor
    ~Vector2(); //! deconstructor

    float& operator() (const unsigned short i);//! outputs mData[i]. i must be between 0 and 2
    Vector2 operator+ (Vector2 vector); //! sum of 2 vectors
    Vector2 operator+ (const float scalar); //! sum of vector and scalar constant
    Vector2 operator- (Vector2 vector); //! subtraction of 2 vectors
    Vector2 operator- (const float scalar); //! subtraction of scalar constant
    Vector2 operator* (Vector2 vector); //! element-wise multiplication
    Vector2 operator* (const float scalar); //! multiplication by a scalar value
    float dot(Vector2 vector); //! scalar (dot) product.
    Vector2 cross(Vector2 vector); //! vector (cross) product
    Vector2& operator= (Vector2 vector); //! assignement
    bool operator== (Vector2 vector);//! equality test
    bool operator!= (Vector2 vector); //! not equal test

    float GetMagnitude(); //! returns the vector magnitude
	Vector2 UnitVector();
    void SetComponents(float x, float y); //! sets (or re-sets) the vector components to x, y
    float* GetComponents(); //! provides in output the pointer to mData
    float& X(); //! returns mData[0]
    float& Y(); //! returns mData[1]

    static Vector2 Rotate(float angle, Vector2 vecIn); //! returns a vector equivalent to RotZ * vecIn

    friend std::ostream & operator << (std::ostream &out, Vector2 &vec);
    
private:

    float mData[2];
};

}

#endif // VECTOR2_H
