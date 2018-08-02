/*
 
 Camera Node
 
 */

#ifndef CAMERANODE_H
#define CAMERANODE_H

#include "scene_node.h"
#include "Events/event_manager.h"
#include "Events/events.h"
#include "Math/constants.h"
#include "frustum.h"

namespace NYX {

class Frustum;

class NYX_EXPORT CameraNode : public SceneNode
{
	class CameraListener : public BaseEventListener
    {
		CameraNode *mParent;

    public:
        CameraListener(string name, CameraNode *parent) : 
			BaseEventListener(name),
			mParent(parent)
		{}
		virtual ~CameraListener() {}

        virtual bool ProcessEvent(EventPtr event);

    };

public:

    CameraNode(SceneNode *parent, IRenderer *renderer, std::string name);
    virtual ~CameraNode();

	void SetActive(bool active) { mIsActive = active; }
	bool IsActive() {return mIsActive; }

	void ComputeViewMatrix(); //in gl terms, model it's the camera attitude w.r.t. the world, view is 
	//the inverse of the model matrix, used to rotate world coordinates to view space. This method computes and
	//stores both separately.
	//Matrix4x4 GetModelMatrix() {return mModelMatrix;} // ==GetTransformMatrix
	Matrix4x4 GetViewMatrix() {return mViewMatrix;}

	virtual void ProcessNode();

	virtual void RotateCamera(float angle, eAxis eRotAxis) = 0;
	virtual void RotateCamera(Quaternion rot) = 0;
	virtual void TranslateCamera(Vector3 delta) = 0;
	
	Frustum mFrustum;

protected:

	//Matrix4x4 mModelMatrix; // ==mTransformMatrix
	Matrix4x4 mViewMatrix;
	bool mIsActive;
	EventListenerPtr mEventListener;
};

}

#endif // CAMERANODE_H
