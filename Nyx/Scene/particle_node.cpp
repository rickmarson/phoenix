/*

Particle Effect Node

*/

#include "particle_node.h"
#include "root_node.h"
#include "Renderer/vertex_buffer.h"
#include "Renderer/index_buffer.h"
#include "Renderer/vertex_array_object.h"

namespace NYX {

ParticleNode::ParticleNode(SceneNode *parent, IRenderer *renderer, std::string name) :
	SceneNode(parent, renderer, name),
	mFX(renderer)
{
	
}

ParticleNode::~ParticleNode()
{

}

void ParticleNode::InitializeFX(std::string programName, std::string kernelName, uint size, std::string materialName) 
{ 
	RootNode* root = GetRootNode();
	std::string shaderName;
	Effect* prog = nullptr;
	mFX.Initialize(programName, kernelName, size, materialName, shaderName, &prog);
	root->AddShaderProgram(shaderName, prog);
}

void ParticleNode::ProcessNode()
{
	UpdateState();
	RootNode* root = GetRootNode();

	//not sure if this transform matrix is built correctly for the model stack.
	root->mModelViewStack.PushMatrix(mTransformMatrix.Transpose());
	
	Effect* shader = nullptr;

	if (root->IsInitialised())
	{
		//update particle configuration
		if (root->GetAnimationUpdateState())
			mFX.ExecuteKernel(root->GetAnimationRefreshRateS());

		//render
        mFX.GetVAO()->Bind();
        mFX.GetVertexBuffer()->Bind();
        
        shader = mFX.GetMaterial()->ShaderProg();
		mRenderer->UseShader(shader);

		list<ShaderUniform>& uniforms = mRenderer->GetShaderUniforms();

		LoadUniforms(uniforms, mFX.GetMaterial().get(), root);

		mRenderer->LoadShaderUniforms();

        mFX.GetVertexBuffer()->Draw( IRenderer::E_POINT, mFX.ParticleCount(), 0, false );
        
        mFX.GetVertexBuffer()->Unbind();
        mFX.GetVAO()->Unbind();
        
		mRenderer->UseShader(0); //might be superflous

		mRenderer->UnbindAllTextures();
	}

	//process all children
	if (!mChildren.empty())	
		for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				(*it)->ProcessNode();

	root->mModelViewStack.PopMatrix();
}


}
