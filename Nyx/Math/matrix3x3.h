#ifndef CMATRIX3X3_H
#define CMATRIX3X3_H

/*

A 3x3 Matrix

*/

#include <iostream>

#include "vector3.h"

namespace NYX
{

//! \brief 3x3 square matrix. 
class NYX_EXPORT Matrix3x3
{
public:

    Matrix3x3(); //! empty constructor
    Matrix3x3(float *raw_data); //! constructor. vector components are initialised with x, y, z
    Matrix3x3(const Matrix3x3 &source); //! copy constructor
    ~Matrix3x3(); //! deconstructor

    float& operator() (const unsigned short i, const unsigned short j); //! outputs mData[i*3 + 3]. i and j must be between 0 and 3.
    Matrix3x3 operator+ (Matrix3x3 matrix); //! element-wise sum of 2 matrices
    Matrix3x3 operator+ (const float scalar); //! sum of matrix and scalar constant
    Matrix3x3 operator- (Matrix3x3 matrix); //! element-wise subtraction of 2 matrices
    Matrix3x3 operator- (const float scalar); //! subtraction of scalar constant
    Matrix3x3 operator* (Matrix3x3 matrix); //! matrix (rowsxcolumns) multiplication
    Matrix3x3 operator* (const float scalar); //! multiplication by a scalr value
	Vector3 operator* (Vector3 vector); //! multiplication by a 3d vector
    Matrix3x3& operator= (Matrix3x3 matrix); //! equality test
    bool operator== (const Matrix3x3 matrix);//! inequality test

	void LoadIdentity();
    Matrix3x3 Transpose(); //! returns the transpose of the current matrix
	Vector3 GetRow(const unsigned short i); //! returns the ith row as a CVector3
	Vector3 GetColumn(const unsigned short j); //! returns the jth columns as a CVector3
    float* Data(); //! return the pointer to mData

	void LoadRotX(float angle); //angle in deg
	void LoadRotY(float angle); //angle in deg
	void LoadRotZ(float angle); //angle in deg

    friend std::ostream & operator << (std::ostream &out, Matrix3x3 &mat);
    
private:

    float mData[9];

};

}

#endif // CMATRIX3x3_H
