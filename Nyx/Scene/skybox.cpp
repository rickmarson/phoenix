/*

Sky Box Node

*/

#include "skybox.h"
#include "root_node.h"
#include "Cache/resource_cache.h"
#include "Renderer/vertex_buffer.h"
#include "Renderer/index_buffer.h"
#include "Renderer/vertex_array_object.h"

namespace NYX {

	const SkyBoxNode::SimpleVertex cube[] = { 
		//top face
		{1.0f, 1.0f, 1.0f, 0.0, 1.0f, 0.0, 1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f, 0.0, 1.0f, 0.0, 1.0f, 0.0f},
		{-1.0f, 1.0f, -1.0f, 0.0, 1.0f, 0.0, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f, 0.0, 1.0f, 0.0, 1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f, 0.0, 1.0f, 0.0, 0.0f, 0.0f},
		{-1.0f, 1.0f, 1.0f, 0.0, 1.0f, 0.0, 0.0f, 1.0f},
		//bottom face
		{-1.0f, -1.0f, -1.0f, 0.0, -1.0f, 0.0, 0.0f, 0.0f},
		{1.0f, -1.0f, -1.0f, 0.0, -1.0f, 0.0, 1.0f, 0.0f},
		{1.0f, -1.0f, 1.0f, 0.0, -1.0f, 0.0, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f, 0.0, -1.0f, 0.0, 0.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f, 0.0, -1.0f, 0.0, 0.0f, 0.0f},
		{1.0f, -1.0f, 1.0f, 0.0, -1.0f, 0.0, 1.0f, 1.0f},
		//left face
		{-1.0f, 1.0f, 1.0f, -1.0f, 0.0, 0.0, 1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f, -1.0f, 0.0, 0.0, 1.0f, 0.0f},
		{-1.0f, -1.0f, -1.0f, -1.0f, 0.0, 0.0, 0.0f, 0.0f},
		{-1.0f, 1.0f, 1.0f, -1.0f, 0.0, 0.0, 1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f, -1.0f, 0.0, 0.0, 0.0f, 0.0f},
		{-1.0f, -1.0f, 1.0f, -1.0f, 0.0, 0.0, 0.0f, 1.0f},
		//right face
		{1.0f, -1.0f, -1.0f, 1.0f, 0.0, 0.0, 0.0f, 0.0f},
		{1.0f, 1.0f, -1.0f, 1.0f, 0.0, 0.0, 1.0f, 0.0f},
		{1.0f, 1.0f, 1.0f, 1.0f, 0.0, 0.0, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f, 0.0, 0.0, 1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f, 1.0f, 0.0, 0.0, 0.0f, 1.0f},
		{1.0f, -1.0f, -1.0f, 1.0f, 0.0, 0.0, 0.0f, 0.0f},
		//front face
		{1.0f, -1.0f, 1.0f, 0.0f, 0.0, 1.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 1.0f, 0.0f, 0.0, 1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f, 0.0f, 0.0, 1.0f, 0.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f, 0.0f, 0.0, 1.0f, 0.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f, 0.0f, 0.0, 1.0f, 0.0f, 0.0f},
		{1.0f, -1.0f, 1.0f, 0.0f, 0.0, 1.0f, 1.0f, 0.0f},
		//back face
		{1.0f, -1.0f, -1.0f, 0.0f, 0.0, -1.0f, 1.0f, 0.0f},
		{-1.0f, -1.0f, -1.0f, 0.0f, 0.0, -1.0f, 0.0f, 0.0f},
		{-1.0f, 1.0f, -1.0f, 0.0f, 0.0, -1.0f, 0.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f, 0.0f, 0.0, -1.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, -1.0f, 0.0f, 0.0, -1.0f, 1.0f, 1.0f},
		{1.0f, -1.0f, -1.0f, 0.0f, 0.0, -1.0f, 1.0f, 0.0f}
	};   

SkyBoxNode::SkyBoxNode(SceneNode *parent, IRenderer *renderer, std::string name) :
	SceneNode(parent, renderer, name),
	mScale(1.0f)
{
	memset(mCubeBuffer, 0, sizeof(SimpleVertex));
}

SkyBoxNode::~SkyBoxNode()
{
    delete mVertexBuffer;
    //delete mIndexBuffer;
    
    delete mVAO;
}

void SkyBoxNode::SetScale() 
{ 
	for (int i = 0; i < 36; i++)
	{
		mCubeBuffer[i].position[0] = cube[i].position[0]*mScale;
		mCubeBuffer[i].position[1] = cube[i].position[1]*mScale;
		mCubeBuffer[i].position[2] = cube[i].position[2]*mScale;
	}
}

void SkyBoxNode::Init(std::string cubeTexture, std::string shaderName, float scale)
{
	RootNode* root = GetRootNode();

	mScale = scale;
	mShaderName = shaderName;

	SetScale();

    mVertexBuffer = mRenderer->CreateVertexBuffer();
    //mIndexBuffer = mRenderer->CreateIndexBuffer();
    mVertexBuffer->Create( sizeof(SimpleVertex), 36 );
    mVAO = mRenderer->CreateVertexArrayObject();
    mVAO->Create();
    
	float* vbo_data = mVertexBuffer->Lock();

	if (vbo_data == NULL)
	{
		return;
	}

	memcpy(vbo_data, mCubeBuffer, sizeof(mCubeBuffer));
    mVertexBuffer->Unlock();

	mShader = mRenderer->CreateShaderProgram(shaderName);
	root->AddShaderProgram(shaderName, mShader);
	
    mVAO->Bind();
    mVertexBuffer->Bind();
    
    mVAO->EnableVertexAttribute(mShader, "vVertex", 3, sizeof(float)*8, (sizeof(float)*0));
    //mVAO->EnableVertexAttribute(mShader, "vNormal", 3, sizeof(float)*8, (sizeof(float)*3));
    //mVAO->EnableVertexAttribute(mShader, "vTexCoord", 2, sizeof(float)*8, (sizeof(float)*6));
    
    mVertexBuffer->Unbind();
    mVAO->Unbind();
    
    //init cube map
	ImageResource* hndl = dynamic_cast<ImageResource*>(ResourceCache::GetInstance()->RequestResource(cubeTexture, RES_TEXTURE_CUBE_MAP).get());
	mCubeMap = hndl->GetTexture();
}

void SkyBoxNode::ProcessNode()
{
	//the skybox is by definition a static object.
	//also, it's always a direct child of root, it's at 0,0,0 
	//and it's rotation matrix is always an identity, so there's no point in
	//calculating/pushing a tranform matrix.
	RootNode* root = GetRootNode();

	//render
    mVertexBuffer->Bind();
    //mIndexBuffer->Bind();
    mVAO->Bind();
    
    mRenderer->UseShader(mShader);

	list<ShaderUniform>& uniforms = mRenderer->GetShaderUniforms();

	LoadUniforms(uniforms, NULL, root);

	mRenderer->LoadShaderUniforms();

    mVertexBuffer->Draw( IRenderer::E_TRIANGLE, (uint)36, 0 , false );

	mRenderer->UseShader(0); //might be superflous
    
    mVAO->Unbind();
    mVertexBuffer->Unbind();
    //mIndexBuffer->Unbind();
	
	mRenderer->UnbindAllTextures();

}


}
