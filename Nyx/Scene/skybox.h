/*

Sky Box Node
 
*/

#ifndef SKYBOX_H
#define SKYBOX_H

#include "scene_node.h"
#include "material.h"

namespace NYX {

class Texture;
class VertexBuffer;
class IndexBuffer;
class VertexArrayObject;
class Effect;
    
class NYX_EXPORT SkyBoxNode : public SceneNode
{
public:

	struct SimpleVertex
	{
		float position[3];
		float normal[3];
		float texcoord[2];
	};

    SkyBoxNode(SceneNode *parent, IRenderer *renderer, std::string name);
    ~SkyBoxNode();
	
	void Init(std::string cubeTexture, std::string shaderName, float scale = 1.0);
	Texture* GetCubeMap();

	virtual void ProcessNode();

private:

	float mScale;
	SimpleVertex mCubeBuffer[36];
    VertexBuffer* mVertexBuffer = nullptr;
    IndexBuffer* mIndexBuffer = nullptr;
	Effect* mShader;
    VertexArrayObject* mVAO;
	Texture* mCubeMap;

	std::string mShaderName;

	void SetScale();
};

inline Texture* SkyBoxNode::GetCubeMap() { return mCubeMap; }

}

#endif // SKYBOX_H
