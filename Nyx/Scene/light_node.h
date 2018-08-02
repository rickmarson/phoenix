/*

Light Node

*/

#ifndef LIGHTNODE_H
#define LIGHTNODE_H

#include "scene_node.h"
#include "frustum.h"

namespace NYX {

class NYX_EXPORT LightNode : public SceneNode
{
public:

    LightNode(SceneNode *parent, IRenderer *renderer, std::string name);
    ~LightNode();

	float* GetAmbientColor() {return mAmbientColor;}
	float* GetDiffuseColor() {return mDiffuseColor;}
	float* GetSpecularColor() {return mSpecularColor;}

	void SetAmbientColor(float ambient[4]) {
		memcpy(mAmbientColor, ambient, 4*sizeof(float));
	}

	void SetDiffuseColor(float diffuse[4]) {
		memcpy(mDiffuseColor, diffuse, 4*sizeof(float));
	}

	void SetSpecularColor(float specular[4]) {
		memcpy(mSpecularColor, specular, 4*sizeof(float));
	}

	void SetActive(bool active) {mIsActive = active;}
	void SetDirectional(bool directional) {mIsDirectional = directional;}
	bool IsActive() {return mIsActive;}
	bool IsDirectional() {return mIsDirectional;}

	//////////////////////////////////////////////////
	//Needed for shadow mapping
	void ComputeViewMatrix(); 
	Matrix4x4 GetViewMatrix() {return mViewMatrix;}

	Frustum mFrustum;
	////////////////////////////////////////////////

	virtual void ProcessNode();

protected:

	float mAmbientColor[4];
	float mDiffuseColor[4];
	float mSpecularColor[4];
	
    float mtNearPlane;
    float mFarPlane;
    float mFOV;

	bool mIsActive;
	bool mIsDirectional;

	//needed for shadow mapping
	bool bCastShadow;
	Matrix4x4 mViewMatrix;

	
};

}

#endif // LIGHTNODE_H
