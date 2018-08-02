/*

The Scene Root Node

*/

#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "scene_node.h"
#include "camera_node.h"
#include "light_node.h"
#include "mesh_node.h"
#include "model_node.h"

namespace NYX {

// Required Classes
class Effect;
    
class NYX_EXPORT MatrixStack
{
public:

	MatrixStack() {}
	~MatrixStack() {}

	void PushMatrix(Matrix4x4 m);
	void InsertBottom(Matrix4x4 m);
	//updates the bottom/first/front matrix in the stack.
	//allows easy access and update to the view matrix (and 
	//main projection matrix)
	void UpdatedBottom(Matrix4x4 m);
	Matrix4x4 PopMatrix();

	int GetCurrentStackDepth();

	Matrix4x4 GetCurrentMatrix();
	Matrix4x4 GetViewMatrix();
	Matrix4x4 GetModelMatrix();

private:

	std::list<Matrix4x4> mStack;
};

class NYX_EXPORT RootNode : public SceneNode
{
	class RootListener : public BaseEventListener
    {
		RootNode *mParent;

    public:
        RootListener(string name, RootNode *parent) : 
			BaseEventListener(name),
			mParent(parent)
		{}
		virtual ~RootListener() {}

        virtual bool ProcessEvent(EventPtr event);
    };

	friend class RootListener;

public:

	RootNode(IRenderer *renderer, std::string name = "Root");
	virtual ~RootNode();

	bool IsInitialised();
	void UpdateAnimation(bool update);
	bool GetAnimationUpdateState(); 
	void SetAnimationRefreshRate(uint rate);
	uint GetAnimationRefreshRate(); //returns ms
	float GetAnimationRefreshRateS(); //returns s
	virtual void ProcessNode();
	void SetActiveCamera(CameraNode* camera, const Matrix4x4& viewMatrix);
	void AddCamera(CameraNode* camera);
	void SetCameraPosition(Vector3 pos);
	//used for 3rd-person-style cameras
	void SetCameraTarget(Vector3 target);
	void SetCameraUp(Vector3 up);
	Vector3 GetCameraTarget();
	///////////////////////////////////
	//used for 1st-person-style cameras
	void PitchCamera(float angle);
	void RollCamera(float angle);
	void YawCamera(float angle);
	//////////////////////////////////

	void AddLight(LightNode* light);
	void ReInitScene();
	//deprecated
	void AddMesh(uint meshID, MeshPtr mesh);
	MeshPtr GetMesh(uint meshID);
	//**************
	const std::list<LightNode*>& GetActiveLights();
	void AddTexture(std::string texName, uint texID);
	uint GetTextureId(std::string texName);
	void AddShaderProgram(std::string shaderName, Effect* shader);
	Effect* GetShader(std::string shaderName);

	void AddSkyBox(std::string name, std::string cubeTexture, std::string shaderName, float scale);
	Texture* GetCurrentSkyMap();

	//utility method
	Matrix4x4 GetCurrentMVPMatrix();

	MatrixStack mModelViewStack;
	MatrixStack mProjectionStack;

private:

	std::map<uint, LightNode*> mLights;
	std::list<LightNode*> mActiveLights;
	CameraNode* mActiveCamera;
	std::list<CameraNode*> mCameras;
	std::map<uint, MeshPtr> mMeshCache; //change it to store models?
	std::map<std::string, uint> mTextureCache;
	std::map<std::string, Effect*> mShaderCache;
	EventListenerPtr mEventListener;
	bool mInitialised;
	bool mUpdateAnimation;
	uint mAnimationRefreshRate; //default 20 ms. shuold always match the top level animation refresh rate (game.h)
	SceneNodePtr mSkyBox;
	bool bHasSkyBox;

	void InitScene();
};

typedef std::shared_ptr<RootNode> RootNodePtr;

//Inline method definitions

inline void MatrixStack::PushMatrix(Matrix4x4 m) { mStack.push_back(m); }
inline void MatrixStack::InsertBottom(Matrix4x4 m) { mStack.push_front(m); }
inline int MatrixStack::GetCurrentStackDepth() { return mStack.size();  }
inline Matrix4x4 MatrixStack::GetViewMatrix() { return mStack.front(); }

inline void MatrixStack::UpdatedBottom(Matrix4x4 m)
{
	if (mStack.size() == 0)
		mStack.push_back(m);
	Matrix4x4& front = mStack.front();
	front = m;
}

inline Matrix4x4 MatrixStack::PopMatrix()
{
	Matrix4x4 m;
	if (mStack.size() == 0)
	{	m.LoadIdentity();
		return m;
	}
	m = mStack.back();
	mStack.pop_back();
	return m;
}
	
inline Matrix4x4 MatrixStack::GetCurrentMatrix()
{
	Matrix4x4 m;
	m.LoadIdentity();
	std::list<Matrix4x4>::reverse_iterator rit;
	for (rit = mStack.rbegin(); rit != mStack.rend(); ++rit)
		m = m * (*rit);
	return m;
}

inline Matrix4x4 MatrixStack::GetModelMatrix()
{
	Matrix4x4 m;
	m.LoadIdentity();
	std::list<Matrix4x4>::reverse_iterator rit;
	std::list<Matrix4x4>::reverse_iterator rcheck = mStack.rbegin();
	rcheck++;
	for (rit = mStack.rbegin(); rit != mStack.rend(); ++rit)
	{
		if (rcheck == mStack.rend())
			break;
		m = m * (*rit);
		rcheck++;
	}
	return m;
}

inline bool RootNode::IsInitialised() { return mInitialised; }
inline void RootNode::UpdateAnimation(bool update) { mUpdateAnimation = update; } 
inline bool RootNode::GetAnimationUpdateState() { return mUpdateAnimation; }
inline void RootNode::SetAnimationRefreshRate(uint rate) { mAnimationRefreshRate = rate; }
inline uint RootNode::GetAnimationRefreshRate() { return mAnimationRefreshRate; }
inline float RootNode::GetAnimationRefreshRateS() { return (float)mAnimationRefreshRate/1000.0f; }

inline Vector3 RootNode::GetCameraTarget() { return mActiveCamera->GetLookAt(); }
inline void RootNode::AddLight(LightNode* light) { mLights[light->GetID()] = light; }
inline void RootNode::ReInitScene() {mInitialised = false;}
//deprecated
inline void RootNode::AddMesh(uint meshID, MeshPtr mesh) { mMeshCache[meshID] = mesh; }
inline MeshPtr RootNode::GetMesh(uint meshID) { return mMeshCache[meshID]; }
//**************
inline const std::list<LightNode*>& RootNode::GetActiveLights() { return mActiveLights; }
inline void RootNode::AddTexture(std::string texName, uint texID) { mTextureCache[texName] = texID; }
inline uint RootNode::GetTextureId(std::string texName) { return mTextureCache[texName]; }
inline void RootNode::AddShaderProgram(std::string shaderName, Effect* shader) { mShaderCache[shaderName] = shader; }
inline Effect* RootNode::GetShader(std::string shaderName) { return mShaderCache[shaderName]; }

inline Matrix4x4 RootNode::GetCurrentMVPMatrix() 
{
	Matrix4x4 MV, P;
	MV = mModelViewStack.GetCurrentMatrix();
	P = mProjectionStack.GetCurrentMatrix();
	return (MV*P); //check order
}

inline void RootNode::SetActiveCamera(CameraNode* camera, const Matrix4x4& viewMatrix) 
{ 
	mActiveCamera = camera; 
	//the view matrix is always at the bottom of the stack!
	if (mInitialised)
		mModelViewStack.UpdatedBottom(viewMatrix);
	else
		mModelViewStack.InsertBottom(viewMatrix);
}

inline void RootNode::AddCamera(CameraNode* camera)
{
	//don't add if it's already there
	for (list<CameraNode*>::iterator it = mCameras.begin();
		it != mCameras.end(); ++it)
	{
		if ((*it) == camera)
			return;
	}

	mCameras.push_back(camera);
}

inline void RootNode::SetCameraPosition(Vector3 pos) 
{
	if (NULL == mActiveCamera)
		return;
	mActiveCamera->SetRelativePosition(pos);
}

inline void RootNode::SetCameraTarget(Vector3 target) 
{
	if (NULL == mActiveCamera)
		return;
	mActiveCamera->LookAt(target);
}

inline void RootNode::SetCameraUp(Vector3 up) 
{
	if (NULL == mActiveCamera)
		return;
	mActiveCamera->UpVector(up);
}

inline void RootNode::PitchCamera(float angle)
{
	//TODO
}

inline void RootNode::RollCamera(float angle)
{
	//TODO
}

inline void RootNode::YawCamera(float angle)
{
	//TODO
}

}

#endif // ROOTNODE_H
