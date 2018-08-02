/*
 
 The Scene Root Node
 
 */

#include "root_node.h"
#include "skybox.h"

#include "render_buffer.h"

using namespace std;

namespace NYX {

bool RootNode::RootListener::ProcessEvent(EventPtr event)
{
	 eEventType evT = event->GetEventType();

    //each listeners is already registered only to events of interest,
    //there should be no need to check if the incoming event is valid.

    switch (evT)
    {
    case EV_ACTIVE_CAMERA_CHANGED:
		{
		  CameraChangedEvent* cam_event = dynamic_cast<CameraChangedEvent*>(event.get());
		
		  for (list<CameraNode*>::iterator it = mParent->mCameras.begin();
			  it !=  mParent->mCameras.end(); ++it)
		  {
			  if (cam_event->Id() == (*it)->GetID())
			  {
				  (*it)->SetActive(true);
				  mParent->SetActiveCamera(*it, (*it)->GetViewMatrix());
				  mParent->mProjectionStack.UpdatedBottom((*it)->mFrustum.GetProjectionMatrix());
			  }
			  else
				  (*it)->SetActive(false);
		  }
		}
		break;
    default:
        return false;
        break;
    }

    return true;
}

RootNode::RootNode(IRenderer *renderer, std::string name) :
	SceneNode(NULL, renderer, name),
	mAnimationRefreshRate(20),
	bHasSkyBox(false)
{
	mAbsolutePosition.SetComponents(0.0, 0.0, 0.0);
	mRelativePosition = mAbsolutePosition;
	mToWorld.LoadIdentity();	
	mToParent.LoadIdentity();
	mInitialised = false;
	mEventListener = EventListenerPtr( new RootListener(mName + " - Evt Listener", this) );
	EventManager::GetInstance()->RegisterListener(EV_ACTIVE_CAMERA_CHANGED, mEventListener);
}

RootNode::~RootNode() 
{
	mActiveCamera = NULL;
	for(map<uint, LightNode*>::iterator lights = mLights.begin(); 
		lights != mLights.end(); ++lights)
	{
		(*lights).second = NULL;
	}
	for(list<LightNode*>::iterator lights = mActiveLights.begin(); 
		lights != mActiveLights.end(); ++lights)
	{
		(*lights) = NULL;
	}
	for(list<CameraNode*>::iterator cams = mCameras.begin(); 
		cams != mCameras.end(); ++cams)
	{
		(*cams) = NULL;
	}
}

void RootNode::AddSkyBox(std::string name, std::string cubeTexture, std::string shaderName, float scale)
{
	//sky box is always centered at the world origin
	Vector3 pos(0.0, 0.0, 0.0);
	Matrix3x3 att;
	att.LoadIdentity();

	SceneNodePtr skybox(new SkyBoxNode(dynamic_cast<SceneNode*>(this), mRenderer, "SkyBox")); 
	SkyBoxNode* sky_box_node = dynamic_cast<SkyBoxNode*>(skybox.get());
	sky_box_node->SetRelativePosition(pos);
	sky_box_node->SetLocalAttitude(att);
	sky_box_node->Init(cubeTexture, shaderName, scale);

	mSkyBox = skybox;
	bHasSkyBox = true;
}

Texture* RootNode::GetCurrentSkyMap()
{
	//for now, just the skybox texture map.
	//but it may be a dynamically generated environment map in the future
	return (dynamic_cast<SkyBoxNode*>(mSkyBox.get()))->GetCubeMap();
}
    
//#define TEST_RENDER_BUFFER

#ifdef TEST_RENDER_BUFFER
    RenderBuffer *render_target = nullptr;
#endif
    
void RootNode::ProcessNode()
{
	if (!mInitialised)
		InitScene();

	//set modelview matrix to current transform matrix (i.e. push current local matrix)
	mToWorld.LoadIdentity();
	mAbsolutePosition(0) = 0.0;
	mAbsolutePosition(1) = 0.0;
	mAbsolutePosition(2) = 0.0;
	BuildTransformFromRotation();

	//the intention here was to push this on the bottom of the model stack to provide 
	//a world coordinate transormation (e.g. flip y and z to match "physical" coordinates).

	//set camera 
	//deprecated
    Matrix4x4 mv = mModelViewStack.GetCurrentMatrix();
    Matrix4x4 proj = mProjectionStack.GetCurrentMatrix();
	mRenderer->SetActiveCamera(mv, proj,
		mActiveCamera->mFrustum.GetClipNear(), mActiveCamera->mFrustum.GetClipFar(), 
		mActiveCamera->mFrustum.GetFOV());
    //
    
#ifdef TEST_RENDER_BUFFER
    if ( !render_target )
    {
        render_target = mRenderer->CreateRenderBuffer();
        render_target->Create( RenderBuffer::StorageType::TEXTURE_STORAGE, 1024, 768, true, 4);
    }
    
    render_target->Bind();
#endif
    
    mRenderer->EnableDepthTest();
	mRenderer->BeginScene();

	//set lights
	mActiveLights.clear();
	for(map<uint, LightNode*>::iterator lights = mLights.begin(); 
		lights != mLights.end(); ++lights)
	{
		LightNode* light = (*lights).second;
		
		if (light->IsActive())
		{
			mActiveLights.push_back(light);
		}
	}

	//process all children
	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		(*it)->ProcessNode();

	//render skybox
	if (bHasSkyBox)
	{
		//disable culling (or more optimal, reverse the front face winding)
		mRenderer->DisableCulling();
		mSkyBox->ProcessNode();
		//restore original settings
		mRenderer->EnableCulling();
	}

	//process all reflections
	//process all transparent objects

	mRenderer->EndScene();
    
#ifdef TEST_RENDER_BUFFER
    render_target->Unbind();
    render_target->Blit();
#endif
}

void RootNode::InitScene()
{
	//Need a first "setup" pass trough the scene graph to setup currently active cameras and lights 
	//before the very first render pass.
	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		(*it)->ProcessNode();

	mInitialised = true;
}


}
