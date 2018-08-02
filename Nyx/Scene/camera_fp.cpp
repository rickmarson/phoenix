/*
 
 First Person Camera
 
 */

#include "camera_fp.h"
#include "constants.h"

namespace NYX {

CameraFP::CameraFP(SceneNode *parent, IRenderer *renderer, std::string name) :
	CameraNode(parent, renderer, name)
{

}

CameraFP::~CameraFP()
{

}

void CameraFP::RotateCamera(float angle, eAxis eRotAxis) 
{
	//angle in DEG
	Matrix3x3 rot;
	
	/*Since cameras are generally controlled via look and up vectors, one approach is to apply 
	 the rotation to mLookAt and mUp, respectively.
	 Alternatively, the rotation could also be applied directly to mToParent, but then the 
	 transform matrix should be built by "summing" tranlsation and rotation instead of via
	 the more traditional "look-at" method. 
	 For the moment, it's simpler to just rotate the lookAt/Up vectors and leave the transform
	 matrix build method unchanged across cameras.
	*/
	switch (eRotAxis)
	{
	case AX_X:
		rot.LoadRotX(angle*DEG2RAD);
		mLookAt = rot * mLookAt;
		break;
	case AX_Y:
		rot.LoadRotY(angle*DEG2RAD);
		mLookAt = rot * mUp;
		break;
	case AX_Z:
		rot.LoadRotZ(angle*DEG2RAD);
		mUp = rot * mLookAt;
		break;
	}
}

void CameraFP::RotateCamera(Quaternion rot)  
{
	mLookAt = rot.RotateVector(mLookAt);
}

void CameraFP::TranslateCamera(Vector3 delta) 
{
	//in this case it's a straightforward sum.
	/* note that most often the motion will be constrained on a plane 
	   (i.e. walking on a surface, with no y motion or with y motion governed by terrain),
	   but this is controlled through the delta value passed in by the application, as 
	   this behaviour is application-depend
	*/
	mRelativePosition = mRelativePosition + delta;
}
	
}