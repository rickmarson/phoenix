/*

Mesh Node

*/

#include "mesh_node.h"
#include "root_node.h"
#include "Renderer/vertex_buffer.h"
#include "Renderer/index_buffer.h"
#include "Renderer/vertex_array_object.h"

namespace NYX {

MeshNode::MeshNode(SceneNode *parent, IRenderer *renderer, std::string name, VertexBuffer* vb, IndexBuffer* ib, VertexArrayObject* vao) :
	SceneNode(parent, renderer, name)
{
    mVertexBuffer = vb;
    mIndexBuffer = ib;
    mVertexArray = vao;
}

MeshNode::~MeshNode()
{
    delete mVertexArray;
}

void MeshNode::SetupVAO( Effect* program )
{
    mVertexArray->Create();
    
    mVertexArray->Bind();
    
    mVertexArray->EnableVertexAttribute( program, "vVertex", 3, sizeof(Vertex), offsetof(struct Vertex, position) );
    mVertexArray->EnableVertexAttribute( program, "vNormal", 3, sizeof(Vertex), offsetof(struct Vertex, normal) );
    mVertexArray->EnableVertexAttribute( program, "vTexCoord0", 2, sizeof(Vertex), offsetof(struct Vertex, texCoord) );

    mVertexArray->Unbind();
    
}
    
void MeshNode::ProcessNode()
{
	UpdateState();
	RootNode* root = GetRootNode();

	//not sure if this transform matrix is built correctly for the model stack.
	root->mModelViewStack.PushMatrix(mTransformMatrix.Transpose());
	
	//these declarations must go before the goto statement.
	Effect* shader = nullptr;

	if (root->IsInitialised())
	{
        mVertexArray->Bind();
        mVertexBuffer->Bind();
        mIndexBuffer->Bind();
        
		shader = mMesh->mMaterial->ShaderProg();
		mRenderer->UseShader(shader);

		list<ShaderUniform> &uniformNames = mRenderer->GetShaderUniforms();

		LoadUniforms(uniformNames, mMesh->GetMaterial(), root);

		mRenderer->LoadShaderUniforms();

        mVertexBuffer->Draw(mMesh->PolygonType(), mMesh->PolyCount());
        
        mVertexBuffer->Unbind();
        mIndexBuffer->Unbind();
        mVertexArray->Unbind();
        
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
