/*

A 3x3 Matrix

*/

#include "Constants.h"
#include "Matrix3x3.h"

#include <math.h>

namespace NYX {

//constructors
Matrix3x3::Matrix3x3()
{
    for (unsigned short i = 0; i < 9; ++i)
        mData[i] = 0.0;

}

Matrix3x3::Matrix3x3(float* raw_data)
{
    if (raw_data == NULL)
        return;

    for (unsigned short i = 0; i < 9; ++i)
    {
        mData[i] = raw_data[i];
    }
}

Matrix3x3::Matrix3x3(const Matrix3x3 &source)
{
    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            mData[i*3 + j] = source.mData[i*3 + j];
        }
    }
}

Matrix3x3::~Matrix3x3()
{

}

//operators overloading
float& Matrix3x3::operator() (const unsigned short i, const unsigned short j)
{
    if ( (i < 3) && (j < 3) )
        return mData[i*3 + j];
    else
    {
        std::cout << "Error when calling Matrix3x3 operator(), requested indexes are out of bounds." << std::endl;
        return mData[0];
    }
}

Matrix3x3 Matrix3x3::operator+ (Matrix3x3 matrix)
{
    Matrix3x3 out;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            out(i,j) = mData[i*3 + j] + matrix(i,j);
        }
    }

    return out;
}

Matrix3x3 Matrix3x3::operator+ (const float scalar)
{
    Matrix3x3 out;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            out(i,j) = mData[i*3 + j] + scalar;
        }
    }

    return out;
}

Matrix3x3 Matrix3x3::operator- (Matrix3x3 matrix)
{
    Matrix3x3 out;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            out(i,j) = mData[i*3 + j] - matrix(i,j);
        }
    }

    return out;
}

Matrix3x3 Matrix3x3::operator- (const float scalar)
{
    Matrix3x3 out;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            out(i,j) = mData[i*3 + j] - scalar;
        }
    }

    return out;
}

Matrix3x3 Matrix3x3::operator* (Matrix3x3 matrix)
{
    Matrix3x3 out;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            float tempValue = 0.0;
            unsigned short k = 0;
            while (k < 3)
            {
                tempValue += mData[i*3 + k]*matrix(k,j);
                k++;
            }

            out(i,j) = tempValue;
        }
    }

    return out;
}

Matrix3x3 Matrix3x3::operator* (const float scalar)
{
    Matrix3x3 out;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            out(i,j) = mData[i*3 + j]*scalar;
        }
    }

    return out;
}

Vector3 Matrix3x3::operator* (Vector3 vector)
{
	Vector3 out;

	out.X() = mData[0] * vector.X() + mData[1] * vector.Y() + mData[2] * vector.Z();
	out.Y() = mData[3] * vector.X() + mData[4] * vector.Y() + mData[5] * vector.Z();
	out.Z() = mData[6] * vector.X() + mData[7] * vector.Y() + mData[8] * vector.Z();

	return out;
}

Matrix3x3& Matrix3x3::operator= (Matrix3x3 matrix)
{
    //if the same instance as "this" is passed as argument, return this without performing
    //any operation
    if (this == &matrix)
        return *this;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            mData[i*3 + j] = matrix(i,j);
        }
    }

    return *this;

}

bool Matrix3x3::operator== (Matrix3x3 matrix)
{
     bool equal = true, breakOuterFor = false;

     for (unsigned short i = 0; i < 3; ++i)
     {
         for (unsigned short j = 0; j < 3; ++j)
         {
             if (mData[i*3 + j] != matrix(i,j))
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
void Matrix3x3::LoadIdentity()
{
	 for (unsigned short i = 0; i < 9; ++i)
	 {
        mData[i] = 0.0;

		if ( (i == 0) || (i == 4) || (i == 8) )
			mData[i] = 1.0;
	 }
}


Matrix3x3 Matrix3x3::Transpose()
{
    Matrix3x3 out;

    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            out(i,j) = mData[j*3 + i];
        }
    }

    return out;
}


Vector3 Matrix3x3::GetRow(const unsigned short i)
{
	Vector3 out(mData[i*3], mData[i*3 + 1], mData[i*3 + 2]);

    return out;
}

Vector3 Matrix3x3::GetColumn(const unsigned short j)
{
	Vector3 out(mData[j], mData[3 + j], mData[6 + j]);

    return out;
}

float* Matrix3x3::Data()
{
    return mData;
}

void Matrix3x3::LoadRotX(float angle)
{
	float angRad = angle*DEG2RAD;

	mData[0] = 1.0; mData[1] = 0.0; mData[2] = 0.0;
	mData[3] = 0.0; mData[4] = cos(angRad); mData[5] = -sin(angRad);
	mData[6] = 0.0; mData[7] = sin(angRad); mData[8] = cos(angRad);
}

void Matrix3x3::LoadRotY(float angle)
{
	float angRad = angle*DEG2RAD;

	mData[0] = cos(angRad); mData[1] = 0.0; mData[2] = sin(angRad);
	mData[3] = 0.0; mData[4] = 1.0; mData[5] = 0.0;
	mData[6] = -sin(angRad); mData[7] = 0.0; mData[8] = cos(angRad);
}

void Matrix3x3::LoadRotZ(float angle)
{
	float angRad = angle*DEG2RAD;

	mData[0] = cos(angRad); mData[1] = -sin(angRad); mData[2] = 0.0;
	mData[3] = sin(angRad); mData[4] = cos(angRad); mData[5] = 0.0;
	mData[6] = 0.0; mData[7] = 0.0; mData[8] = 1.0;
}

std::ostream & operator << (std::ostream &out, Matrix3x3 &mat)
{
    for (unsigned short i = 0; i < 3; ++i)
    {
        for (unsigned short j = 0; j < 3; ++j)
        {
            out << mat(i,j) << '\t';
        }
        out << std::endl;
    }
    
    return out;
        
}
    
}