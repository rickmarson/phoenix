/*

Third Person Camera
 
*/

#include "camera_tp.h"
#include "constants.h"
#include <math.h>

namespace NYX {

CameraTP::CameraTP(SceneNode *parent, IRenderer *renderer, std::string name) :
	CameraNode(parent, renderer, name),
	rotAngX(0),
	rotAngY(0)
{
	
}

CameraTP::~CameraTP()
{

}

void CameraTP::RotateCamera(float angle, eAxis eRotAxis) 
{
	//third-person camera rotation is governed by the target (look-at) point
	//and it's not controlled directly.

	//should leave the "unused variable" warnings there?
}

void CameraTP::RotateCamera(Quaternion rot)  
{
	//third-person camera rotation is governed by the target (look-at) point
	//and it's not controlled directly.

	//should leave the "unused variable" warnings there?
}

void CameraTP::TranslateCamera(Vector3 delta) 
{
	//follow the opengl coordinate system convention
	//remember that deltas are all relative to the body axis (not world axis)
	if (delta.X() != 0 || delta.Y() != 0) 
	{
		//move camera on the surface of a sphere of constant radius centered on the target
		float R, x, y, z;
		R = mRelativePosition.GetMagnitude();
		rotAngX += delta.X()*NYX::DEG2RAD;
		rotAngY += delta.Y()*NYX::DEG2RAD;

		x = R*sin(rotAngX)*cos(rotAngY);
		z = R*cos(rotAngX)*cos(rotAngY);
		y = R*sin(rotAngY);

		mRelativePosition.SetComponents(x, y, z);

	}
	else if (delta.Z() != 0)
	{
		//zoom the camera in/out w.r.t. target.
		Vector3 newPos;
		float R;

		R = mRelativePosition.GetMagnitude();
	    R += delta.Z();

		//might need to clip somehow
		/*if (R > 20.0)
			R = 20.0;

		if (R < 6.0)
			R = 6.0;*/

		if (bHasTarget)
			mLookAt = mTarget->GetAbsolutePosition() - mAbsolutePosition;

		newPos = mLookAt.UnitVector() * R;

		//update camera
		mRelativePosition = newPos;
	}
	
}

}