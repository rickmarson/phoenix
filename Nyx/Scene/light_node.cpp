/*

Light Node

*/

#include "light_node.h"
#include "root_node.h"

namespace NYX {

LightNode::LightNode(SceneNode *parent, IRenderer *renderer, std::string name) :
	SceneNode(parent, renderer, name)
{
	bUseLookAt = true;
	bCastShadow = false;
}

LightNode::~LightNode()
{
	int dummy = 0;
}

void LightNode::ProcessNode()
{
	UpdateState();
	//if casting shadow, compute view matrix
	if (bCastShadow)
		ComputeViewMatrix();

	GetRootNode()->AddLight(this);

	//process all children
	if (!mChildren.empty())	
		for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				(*it)->ProcessNode();
}

void LightNode::ComputeViewMatrix()
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

}