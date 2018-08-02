/*

A 4x4 Matrix

*/
#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "vector4.h"
#include <iostream>

namespace NYX
{

//! \brief 4x4 square matrix. it is primarily intended to be used in the generation of shadow maps.
class NYX_EXPORT Matrix4x4
{
public:

    Matrix4x4(); //! empty constructor
    Matrix4x4(float *raw_data); //! constructor. vector components are initialised with x, y, z
    Matrix4x4(const Matrix4x4 &source); //! copy constructor
    ~Matrix4x4(); //! deconstructor

    float& operator() (const unsigned short i, const unsigned short j); //! outputs mData[i*4 + j]. i and j must be between 0 and 3.
    Matrix4x4 operator+ (Matrix4x4 matrix); //! element-wise sum of 2 matrices
    Matrix4x4 operator+ (const float scalar); //! sum of matrix and scalar constant
    Matrix4x4 operator- (Matrix4x4 matrix); //! element-wise subtraction of 2 matrices
    Matrix4x4 operator- (const float scalar); //! subtraction of scalar constant
    Matrix4x4 operator* (Matrix4x4 matrix); //! matrix (rowsxcolumns) multiplication
    Matrix4x4 operator* (const float scalar); //! multiplication by a scalr value
	Vector4 operator* (Vector4 vector); //! multiplication by a 4d vector
    Matrix4x4& operator= (Matrix4x4 matrix); //! assignement 
    bool operator== (const Matrix4x4 matrix);//! equality test

	void LoadIdentity();
    Matrix4x4 Transpose(); //! returns the transpose of the current matrix
	Matrix4x4 Inverse(float &det); //! returns the inverse of the current matrix and the matrix determinant
    float* GetRow(const unsigned short i); //! returns the ith row as a float*, intended for use with OpenGL API.
    float* Data(); //! returns a pointer to mData. Intended for use with OpenGL API.

    friend std::ostream & operator << (std::ostream &out, Matrix4x4 &mat);
    
private:

    float mData[16];

};

}

#endif // MATRIX4x4_H
