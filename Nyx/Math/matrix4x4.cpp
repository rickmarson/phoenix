/*

A 4x4 Matrix

*/

#include "Constants.h"
#include "Matrix4x4.h"

#include <math.h>

namespace NYX {

//constructors
Matrix4x4::Matrix4x4()
{
    for (unsigned short i = 0; i < 16; ++i)
        mData[i] = 0.0;

}

Matrix4x4::Matrix4x4(float* raw_data)
{
    if (raw_data == NULL)
        return;

    for (unsigned short i = 0; i < 16; ++i)
    {
        mData[i] = raw_data[i];
    }
}

Matrix4x4::Matrix4x4(const Matrix4x4 &source)
{
    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            mData[i*4 + j] = source.mData[i*4 + j];
        }
    }
}

Matrix4x4::~Matrix4x4()
{

}

//operators overloading
float& Matrix4x4::operator() (const unsigned short i, const unsigned short j)
{
    if ( (i < 4) && (j < 4) )
        return mData[i*4 + j];
    else
    {
        std::cout << "Error when calling Matrix4x4 operator(), requested indexes are out of bounds." << std::endl;
        return mData[0];
    }
}

Matrix4x4 Matrix4x4::operator+ (Matrix4x4 matrix)
{
    Matrix4x4 out;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            out(i,j) = mData[i*4 + j] + matrix(i,j);
        }
    }

    return out;
}

Matrix4x4 Matrix4x4::operator+ (const float scalar)
{
    Matrix4x4 out;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            out(i,j) = mData[i*4 + j] + scalar;
        }
    }

    return out;
}

Matrix4x4 Matrix4x4::operator- (Matrix4x4 matrix)
{
    Matrix4x4 out;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            out(i,j) = mData[i*4 + j] - matrix(i,j);
        }
    }

    return out;
}

Matrix4x4 Matrix4x4::operator- (const float scalar)
{
    Matrix4x4 out;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            out(i,j) = mData[i*4 + j] - scalar;
        }
    }

    return out;
}

Matrix4x4 Matrix4x4::operator* (Matrix4x4 matrix)
{
    Matrix4x4 out;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            float tempValue = 0.0;
            unsigned short k = 0;
            while (k < 4)
            {
                tempValue += mData[i*4 + k]*matrix(k,j);
                k++;
            }

            out(i,j) = tempValue;
        }
    }

    return out;
}

Matrix4x4 Matrix4x4::operator* (const float scalar)
{
    Matrix4x4 out;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            out(i,j) = mData[i*4 + j]*scalar;
        }
    }

    return out;
}

Vector4 Matrix4x4::operator* (Vector4 vector)
{
	Vector4 out;

	out.X() = mData[0] * vector.X() + mData[1] * vector.Y() + mData[2] * vector.Z() + mData[3] * vector.W();
	out.Y() = mData[4] * vector.X() + mData[5] * vector.Y() + mData[6] * vector.Z() + mData[7] * vector.W();
	out.Z() = mData[8] * vector.X() + mData[9] * vector.Y() + mData[10] * vector.Z() + mData[11] * vector.W();
	out.W() = mData[12] * vector.X() + mData[13] * vector.Y() + mData[14] * vector.Z() + mData[15] * vector.W();

	return out;
}

Matrix4x4& Matrix4x4::operator= (Matrix4x4 matrix)
{
    //if the same instance as "this" is passed as argument, return this without performing
    //any operation
    if (this == &matrix)
        return *this;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            mData[i*4 + j] = matrix(i,j);
        }
    }

    return *this;

}

bool Matrix4x4::operator== (Matrix4x4 matrix)
{
     bool equal = true, breakOuterFor = false;

     for (unsigned short i = 0; i < 4; ++i)
     {
         for (unsigned short j = 0; j < 4; ++j)
         {
             if (mData[i*4 + j] != matrix(i,j))
             {
                 equal = false;
                 breakOuterFor = true;
                 break;
             }

             if (breakOuterFor)
                 break;
         }
     }

     return equal;
}

//other functions
void Matrix4x4::LoadIdentity()
{
	 for (unsigned short i = 0; i < 16; ++i)
	 {
        mData[i] = 0.0;

		if ( (i == 0) || (i == 5) || (i == 10) || (i == 15) )
			mData[i] = 1.0;
	 }
}


Matrix4x4 Matrix4x4::Transpose()
{
    Matrix4x4 out;

    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            out(i,j) = mData[j*4 + i];
        }
    }

    return out;
}

//modified from the MESA implementation of the GLU library.
//ugly but optimized. 
Matrix4x4 Matrix4x4::Inverse(float &det)
{
	Matrix4x4 inv;
	float *m = mData;
	float detInv;

	inv(0,0) = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv(1,0) = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv(2,0) = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv(3,0) = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv(0,1) = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv(1,1) = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv(2,1) = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv(3,1) = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv(0,2) = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv(1,2) = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv(2,2) = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv(3,2) = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv(0,3) = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv(1,3) = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv(2,3) = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv(3,3) = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv(0,0) + m[1] * inv(1,0) + m[2] * inv(2,0) + m[3] * inv(3,0);

    if (det == 0)
		return inv;

    detInv = 1.0 / det;

	//might need to look at optimizing this double for loop
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			inv(i,j) = inv(i,j) * detInv;

    return inv;
}

float* Matrix4x4::GetRow(const unsigned short i)
{
    return &mData[i*4];
}

float* Matrix4x4::Data()
{
    return mData;
}

std::ostream & operator << (std::ostream &out, Matrix4x4 &mat)
{
    for (unsigned short i = 0; i < 4; ++i)
    {
        for (unsigned short j = 0; j < 4; ++j)
        {
            out << mat(i,j) << '\t';
        }
        out << std::endl;
    }
    
    return out;
}

}