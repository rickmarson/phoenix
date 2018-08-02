/*

Frustum

*/

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Math/matrix4x4.h"
#include "Math/vector3.h"

namespace NYX {

class CameraNode;
class LightNode;

class NYX_EXPORT Frustum
{
	friend class CameraNode;
	friend class LightNode;

public:

	Frustum() {}
	Frustum(float clipNear, float clipFar, float fov, float ar);
	Frustum(float zmin, float zmax, float xmin, float xmax, float ymin, float ymax);
	~Frustum() {}

	float GetFOV() {return mFOV;}
	float GetClipNear() {return mClipNear;}
	float GetClipFar() {return mClipFar;}
	void SetPerspective(float clipNear, float clipFar, float fov, float ar);
	void SetOrthographic(float zmin, float zmax, float xmin, float xmax, float ymin, float ymax);

	bool IsOrtho() {return mIsOrtho;}

	//used to be const
	Matrix4x4& GetProjectionMatrix() { return mProjectionMatrix; } //alternatively perspective or orthographic
	bool TestVisibility(Vector3 center, float radius = 0.0); 

private:

	// perspective params
	float mClipNear; //zMin for ortho
	float mClipFar; //zMax for ortho
	float mFOV;
	float mAspectRatio;
	//ortho params
	float mXMin;
	float mXMax;
	float mYMin;
	float mYMax;

	bool mIsOrtho;

	Matrix4x4 mProjectionMatrix;

	//Frustum points (base/untransformed)
	//near plane
	Vector4 mNearUL, mNearUR, mNearLL, mNearLR;
	//far plane
	Vector4 mFarUL, mFarUR, mFarLL, mFarLR;
	//Frustum points (transformed)
	//near plane
	Vector4 mNearULT, mNearURT, mNearLLT, mNearLRT;
	//far plane
	Vector4 mFarULT, mFarURT, mFarLLT, mFarLRT;

	//plane normals/equations (current/tranformed only)
	//used internally for visibility testing
	Vector4 mNearPlane, mFarPlane, mLeftPlane, mRightPlane, mTopPlane, mBottomPlane;

	void ComputeProjectionMatrix();
};

}

#endif // FRUSTUM_H
