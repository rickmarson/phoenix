/*

Frustum
 
*/

#include "frustum.h"
#include "constants.h"
#include <math.h>

namespace NYX {

Frustum::Frustum(float clipNear, float clipFar, float fov, float ar) 
{
		mClipNear = clipNear;
		mClipFar = clipFar;
		mFOV = fov;
		mAspectRatio = ar;
		mIsOrtho = false;
		ComputeProjectionMatrix();
}
	
Frustum::Frustum(float zmin, float zmax, float xmin, float xmax, float ymin, float ymax) 
{
		mClipNear = zmin;
		mClipFar = zmax;
		mXMin = xmin;
		mXMax = xmax;
		mYMin = ymin;
		mYMax = ymax;
		mIsOrtho = true;
		ComputeProjectionMatrix();
}

void Frustum::ComputeProjectionMatrix()
{
	if (!mIsOrtho)
	{
		//perspective projection
		//similar to gluPerspective
		mProjectionMatrix.LoadIdentity();

		//FOV is in deg
		float fovr = (mFOV * DEG2RAD);
		//near plane
        float yMax = mClipNear * tanf( fovr * 0.5);
		float yMin = -yMax;
		float xMin = yMin * mAspectRatio;
		float xMax = -xMin; 
    
		//construct near plane frustum points
		// Near Upper Left
		mNearUL(0) = xMin; mNearUL(1) = yMax; mNearUL(2) = -mClipNear; mNearUL(3) = 1.0f;
		// Near Lower Left
		mNearLL(0) = xMin; mNearLL(1) = yMin; mNearLL(2) = -mClipNear; mNearLL(3) = 1.0f;
		// Near Upper Right
		mNearUR(0) = xMax; mNearUR(1) = yMax; mNearUR(2) = -mClipNear; mNearUR(3) = 1.0f;
		// Near Lower Right
		mNearLR(0) = xMax; mNearLR(1) = yMin; mNearLR(2) = -mClipNear; mNearLR(3) = 1.0f;

		//construct the projection matrix
		mProjectionMatrix(0,0) = (2.0 * mClipNear) / (xMax - xMin);
		mProjectionMatrix(1,1) = (2.0 * mClipNear) / (yMax - yMin);
		mProjectionMatrix(2,0) = (xMax + xMin) / (xMax - xMin);
		mProjectionMatrix(2,1) = (yMax + yMin) / (yMax - yMin);
		mProjectionMatrix(2,2) = -((mClipFar + mClipNear) / (mClipFar - mClipNear));
		mProjectionMatrix(2,3) = -1.0;
		mProjectionMatrix(3,2) = -((2.0 * (mClipFar*mClipNear))/(mClipFar - mClipNear));
		mProjectionMatrix(3,3) = 0.0;

		//far plane
		yMax = mClipFar * tan(fovr);
        yMin = -yMax;
		xMin = yMin * mAspectRatio;
		xMax = -xMin;
		
		//construct far plane frustum points
		// Far Upper Left
		mFarUL(0) = xMin; mFarUL(1) = yMax; mFarUL(2) = -mClipFar; mFarUL(3) = 1.0f;
		// Far Lower Left
		mFarLL(0) = xMin; mFarLL(1) = yMin; mFarLL(2) = -mClipFar; mFarLL(3) = 1.0f;
		// Far Upper Right
		mFarUR(0) = xMax; mFarUR(1) = yMax; mFarUR(2) = -mClipFar; mFarUR(3) = 1.0f;
		// Far Lower Right
		mFarLR(0) = xMax; mFarLR(1) = yMin; mFarLR(2) = -mClipFar; mFarLR(3) = 1.0f;
	}
	else
	{
		//orthographic projection
		//inputs: float xMin, float xMax, float yMin, float yMax, float zMin, float zMax
		mProjectionMatrix.LoadIdentity();
	
		mProjectionMatrix(0,0) = 2.0 / (mXMax - mXMin);
		mProjectionMatrix(1,2) = 2.0 / (mYMax - mYMin);
		mProjectionMatrix(2,2) = -2.0 / (mClipFar - mClipNear);
		mProjectionMatrix(3,0) = -((mXMax + mXMin)/(mXMax - mXMin));
		mProjectionMatrix(3,1) = -((mYMax + mYMin)/(mYMax - mYMin));
		mProjectionMatrix(3,2) = -((mClipFar + mClipNear)/(mClipFar - mClipNear));
		mProjectionMatrix(3,3) = 1.0;
	}
}

void Frustum::SetPerspective(float clipNear, float clipFar, float fov, float ar) 
{
		mClipNear = clipNear;
		mClipFar = clipFar;
		mFOV = fov;
		mAspectRatio = ar;
		mIsOrtho = false;
		ComputeProjectionMatrix();
}

void Frustum::SetOrthographic(float zmin, float zmax, float xmin, float xmax, float ymin, float ymax) 
{
		mClipNear = zmin;
		mClipFar = zmax;
		mXMin = xmin;
		mXMax = xmax;
		mYMin = ymin;
		mYMax = ymax;
		mIsOrtho = true;
		ComputeProjectionMatrix();
}

bool Frustum::TestVisibility(Vector3 center, float radius)
{
	float dist;

	// Near Plane - See if it is behind me
	dist =ComputeDistanceToPlane(center, mNearPlane);
	if(dist + radius <= 0.0)
		return false;

	// Distance to far plane
	dist = ComputeDistanceToPlane(center, mFarPlane);
	if(dist + radius <= 0.0)
		return false;

	dist = ComputeDistanceToPlane(center, mLeftPlane);
	if(dist + radius <= 0.0)
		return false;

	dist = ComputeDistanceToPlane(center, mRightPlane);
	if(dist + radius <= 0.0)
		return false;

	dist = ComputeDistanceToPlane(center, mBottomPlane);
	if(dist + radius <= 0.0)	
		return false;

	dist = ComputeDistanceToPlane(center, mTopPlane);
	if(dist + radius <= 0.0)
		return false;

	return true;
}

}