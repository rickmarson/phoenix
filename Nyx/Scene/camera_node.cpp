/*

Camera Node

*/

#include "camera_node.h"
#include "root_node.h"
#include "constants.h"
#include "frustum.h"

namespace NYX {

bool CameraNode::CameraListener::ProcessEvent(EventPtr event)
{
	 eEventType evT = event->GetEventType();

    //each listeners is already registered only to events of interest,
    //there should be no need to check if the incoming event is valid.

    CameraMovedEvent *cam_event = NULL;

    switch (evT)
    {
    case EV_CAMERA_MOVED:
		//Don't move this camera if it's not active!
		if (!mParent->IsActive())
			return false;

		cam_event = dynamic_cast<CameraMovedEvent*>(event.get());
		if (!cam_event->bIsRot)
			mParent->TranslateCamera(cam_event->Position());
		else
		{
			if (!cam_event->bUseQuaternion)
				mParent->RotateCamera(cam_event->RotAngle(), cam_event->RotAxis());
			else
				mParent->RotateCamera(cam_event->Rotation());
		}
        break;
    default:
        return false;
        break;
    }

    return true;
}

CameraNode::CameraNode(SceneNode *parent, IRenderer *renderer, std::string name) :
	SceneNode(parent, renderer, name)
{
	//override the default value
	bUseLookAt = true;
	mEventListener = EventListenerPtr( new CameraListener(mName + " - Evt Listener", this) );
	EventManager::GetInstance()->RegisterListener(EV_CAMERA_MOVED, mEventListener);
}

CameraNode::~CameraNode()
{
	EventManager::GetInstance()->UnregisterListener(mEventListener);
}

void CameraNode::ComputeViewMatrix()
{
	// Transform the frustum corners
	mFrustum.mNearULT = mTransformMatrix * mFrustum.mNearUL;
	mFrustum.mNearLLT = mTransformMatrix * mFrustum.mNearLL;
	mFrustum.mNearURT = mTransformMatrix * mFrustum.mNearUR;
	mFrustum.mNearLRT = mTransformMatrix * mFrustum.mNearLR;
	mFrustum.mFarULT = mTransformMatrix * mFrustum.mFarUL;
	mFrustum.mFarLLT = mTransformMatrix * mFrustum.mFarLL;
	mFrustum.mFarURT = mTransformMatrix * mFrustum.mFarUR;
	mFrustum.mFarLRT = mTransformMatrix * mFrustum.mFarLR;

	////////////////////////////////////////////////////
	// Derive Plane Equations from points... Points given in
	// counter clockwise order to make normals point inside 
	// the Frustum
	// Near and Far Planes
	mFrustum.mNearPlane = ComputePlaneEq(mFrustum.mNearULT, mFrustum.mNearLLT, mFrustum.mNearLRT);
	mFrustum.mFarPlane = ComputePlaneEq(mFrustum.mFarULT, mFrustum.mFarURT, mFrustum.mFarLRT);
            
	// Top and Bottom Planes
	mFrustum.mTopPlane = ComputePlaneEq(mFrustum.mNearULT, mFrustum.mNearURT, mFrustum.mFarURT);
	mFrustum.mBottomPlane = ComputePlaneEq(mFrustum.mNearLLT, mFrustum.mFarLLT, mFrustum.mFarLRT);

	// Left and right planes
	mFrustum.mLeftPlane = ComputePlaneEq(mFrustum.mNearLLT, mFrustum.mNearULT, mFrustum.mFarULT);
	mFrustum.mRightPlane = ComputePlaneEq(mFrustum.mNearLRT, mFrustum.mFarLRT, mFrustum.mFarURT);

	//*******************
	//compute view matrix
	float det;
	mViewMatrix = mTransformMatrix.Inverse(det);

	if (det == 0)
		LogManager::GetInstance()->LogMessage("Warning: Matrix Inversion Failed! ( caller: CameraNode::ComputeViewMatrix() )");

}

void CameraNode::ProcessNode()
{
	RootNode* root = GetRootNode();

	//update particle configuration
	if (bScriptAnimated && root->GetAnimationUpdateState())
		UpdateStateFromScript(root->GetAnimationRefreshRateS());

	UpdateState(true); //use absolute position / rotation
	ComputeViewMatrix();

	if (mIsActive)	
		root->SetActiveCamera(this, mViewMatrix);

	//process all children
	//cameras should not have children as it uses a different transform matrix that can't be pushed normally into the stack.
	//i.e. its transform matrix is always absolute. 
	//if (!mChildren.empty())	
	//	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
	//		(*it)->ProcessNode();
}

}