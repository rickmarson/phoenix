/*

3D Model Node

*/

#include "model_node.h"
#include "root_node.h"
#include "Cache/resource_cache.h"
#include "Renderer/vertex_buffer.h"
#include "Renderer/index_buffer.h"
#include "Renderer/vertex_array_object.h"

namespace NYX {

bool ModelNode::ModelListener::ProcessEvent(EventPtr event)
{
	eEventType evT = event->GetEventType();

    //each listeners is already registered only to events of interest,
    //there should be no need to check if the incoming event is valid.

    switch (evT)
    {
    case EV_OBJECT_MOVED:
	{
		ObjectMovedEvent* move_event = dynamic_cast<ObjectMovedEvent*>(event.get());
		
		// ignore event if it's not related to this scenenode. 
		if (move_event->ID() != mParent->GetID())
			return false;

		mParent->SetAbsolutePosition(move_event->Position());

		if (!move_event->bUseQuaternion)
		{
			mParent->SetWorldAttitude(move_event->AttitudeMatrix());
		}
		else
		{
			//mParent->SetWorldAttitude(move_event->Rotation().GetMatrix());
		}
		  
		if (move_event->bIsSoft)
		{
			ModelNode *modelParent = (ModelNode*)mParent;
			float *gl_mesh = modelParent->mVertexBuffer->Lock();
			float *mesh = move_event->GetMesh();
			
			//updated vertices positions and normals with the deformed information from bullet
			int stride = sizeof(Vertex);
			for (int i = 0; i < modelParent->mModel->getNumberOfVertices(); i++)
			{
				//position
				gl_mesh[i*stride + 0] = mesh[i*6 + 0];
				gl_mesh[i*stride + 1] = mesh[i*6 + 1];
				gl_mesh[i*stride + 2] = mesh[i*6 + 2];
				//normal
				gl_mesh[i*stride + 3] = mesh[i*6 + 3];
				gl_mesh[i*stride + 4] = mesh[i*6 + 4];
				gl_mesh[i*stride + 5] = mesh[i*6 + 5];
			}

			modelParent->mVertexBuffer->Unlock();
			mesh = NULL;
			gl_mesh = NULL;
		}
	}
		break;
    default:
        return false;
        break;
    }

    return true;
}

ModelNode::ModelNode(SceneNode *parent, IRenderer *renderer, std::string name) :
	SceneNode(parent, renderer, name)
{
	//override scenenode listener
	EventManager::GetInstance()->UnregisterListener(mEventListener);
	mEventListener.reset();
	mEventListener = EventListenerPtr( new ModelListener(mName + " - Evt Listener", this) );
	EventManager::GetInstance()->RegisterListener(EV_OBJECT_MOVED, mEventListener);
}

ModelNode::~ModelNode()
{
	delete mVertexBuffer;
	delete mIndexBuffer;
}

void ModelNode::AssignModel(std::string modelName)
{
	ObjResource* mhndl = dynamic_cast<ObjResource*>(ResourceCache::GetInstance()->RequestResource(modelName, RES_MODEL).get());
	ModelPtr model = mhndl->GetModel();

	mModel = model;
    if ( scale_factor > 0.0f )
    {
        float offset[4] = {0.0,0.0,0.0,0.0};
        mModel->scale(scale_factor, offset);
    }
    
	BuildChildren();
}

void ModelNode::SetScaleFactor( float scale )
{
    scale_factor = scale;
}
    
void ModelNode::BuildChildren()
{
	RootNode* root = GetRootNode();

    mVertexBuffer = mRenderer->CreateVertexBuffer();
    mIndexBuffer = mRenderer->CreateIndexBuffer();
    
    mVertexBuffer->Create( mModel->getVertexSize(), mModel->getNumberOfVertices(), (void*)mModel->getVertexBuffer() );
    mIndexBuffer->Create( mModel->getIndexSize(), mModel->getNumberOfIndices(), (void*)mModel->getIndexBuffer() );

    mVertexBuffer->Bind();
    
	for (int i = 0; i < mModel->getNumberOfMeshes(); i++)
	{
		MeshPtr mesh = mModel->getMesh(i); 
		string shaderName = mesh->GetMaterial()->GetShaderName();

		Effect* program = mRenderer->CreateShaderProgram(shaderName);
		root->AddShaderProgram(shaderName, program);
		mesh->GetMaterial()->SetShaderProg(program);
		
        // this is per mesh
        VertexArrayObject* vao = mRenderer->CreateVertexArrayObject();
        
        SceneNodePtr mesh_node_ptr(new MeshNode(dynamic_cast<SceneNode*>(this), mRenderer, mName + " mesh " + std::to_string(i), mVertexBuffer, mIndexBuffer, vao ));
		MeshNode* mesh_node = dynamic_cast<MeshNode*>(mesh_node_ptr.get());
		mesh_node->AssignMesh(mesh);
        
        mesh_node->SetupVAO( program );
        
		mChildren.push_back(mesh_node_ptr);

		//TODO: check each mesh for special attributes (transparency/shadows/reflections) and add it to the appropriate list 
		//if necessary.
	}
    
    mVertexBuffer->Unbind();

}

void ModelNode::ProcessNode()
{
	UpdateState(bIsDynamic);
	RootNode* root = GetRootNode();

	//not sure if this transform matrix is built correctly for the model stack.
	root->mModelViewStack.PushMatrix(mTransformMatrix.Transpose());
	
	//process all children
	if (!mChildren.empty())	
		for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				(*it)->ProcessNode();

	root->mModelViewStack.PopMatrix();
}


}
