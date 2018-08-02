/*
 
 Generic Scene Node
 
 */

#include "scene_node.h"
#include "root_node.h"
#include "Script/script_manager.h"
#include "Utils/hash.h"

using namespace LuaPlus;

namespace NYX
{

SceneNode::SceneNode(SceneNode *parent, IRenderer *renderer, std::string name) :
    mParent(parent),
    mName(name),
    mRenderer(renderer),
    bUseLookAt(false),
    bHasTarget(false),
    bIsDynamic(false),
    mTarget(NULL),
    bScriptAnimated(false)
{
    mToWorld.LoadIdentity();
    mToParent.LoadIdentity();
        
    if (parent == NULL) //root node
    {
        mID = 0;
        mNodeType = SCENE_ROOT_NODE;
    }
    else
    {
        mID = GenerateHash( name.c_str(), name.length() );
    }
    
    mEventListener = EventListenerPtr( new NodeListener(mName + " - Evt Listener", this) );
    EventManager::GetInstance()->RegisterListener(EV_OBJECT_MOVED, mEventListener);
}
    
SceneNode::~SceneNode()
{
    mParent = NULL;
        
    for (list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
    {
        (*it).reset();
    }
        
    if (bHasTarget)
        mTarget = NULL;
        
    //list is destroyed automatically, and list members deconstructor is called,
    //resetting the usage count of shared_ptr automatically.
}

bool SceneNode::NodeListener::ProcessEvent(EventPtr event)
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
		  
	}
		break;
    default:
        return false;
        break;
    }

    return true;
}

RootNode* SceneNode::GetRootNode()
{
	RootNode *root = NULL;
	SceneNode *temp = mParent;

	while (temp->GetParentNode())
		temp = temp->GetParentNode();
	
	root = dynamic_cast<RootNode*>(temp);
	
	return root;
}

void SceneNode::BuildTransformFromRotation(bool useAbsoluteCoords)
{
	for(int i = 0; i < 4; i++)
	{
		if (i != 3 && useAbsoluteCoords)
		{
			mTransformMatrix(i, 0) = mToWorld(i,0);
			mTransformMatrix(i, 1) = mToWorld(i,1);
			mTransformMatrix(i, 2) = mToWorld(i,2);
			mTransformMatrix(i, 3) = mAbsolutePosition(i);
		}
		else if (i != 3 && !useAbsoluteCoords)
		{
			mTransformMatrix(i, 0) = mToParent(i,0);
			mTransformMatrix(i, 1) = mToParent(i,1);
			mTransformMatrix(i, 2) = mToParent(i,2);
			mTransformMatrix(i, 3) = mRelativePosition(i);
		}
		else	
		{
			mTransformMatrix(i, 0) = 0.0;
			mTransformMatrix(i, 1) = 0.0;	
			mTransformMatrix(i, 2) = 0.0;
			mTransformMatrix(i, 3) = 1.0;
		}
	}
}

void SceneNode::BuildTransformFromLookAt(bool useAbsolutePosition)
{
	//*******************************
	//computes the camera world attitude 
	//(model) matrix from position, target and up vectors.
	Vector3 forward, up, cross;

	// From OpenGL Superbible 5:
	///////////////////////////////////////////////////////////////////
	// Create the transformation matrix. This was the trickiest part
	// for me. The default view from OpenGL is down the negative Z
	// axis. However, building a transformation axis from these 
	// directional vectors points the frustum the wrong direction. So
	// You must reverse them here, or build the initial frustum
	// backwards - which to do is purely a matter of taste. I chose to
	// compensate here to allow better operability with some of my other
	// legacy code and projects. RSW
	////////////////////////////////////////////////////////////////////

	//update look at vector
	Vector3 eye;

	if (bHasTarget)
	{
		if (useAbsolutePosition)
			eye = mTarget->GetAbsolutePosition() - mAbsolutePosition;
		else
		{
			Vector3 temp = mTarget->GetAbsolutePosition() - mParent->GetAbsolutePosition();
			eye = temp - mRelativePosition;
		}
	}
	else
	{
		if (useAbsolutePosition)
			eye = mLookAt - mAbsolutePosition;
		else
		{
			Vector3 temp = mLookAt - mParent->GetAbsolutePosition();
			eye = temp - mRelativePosition;
		}
	}
		
	forward = eye.UnitVector();
	up = mUp.UnitVector();
	
	//********************************
	//is the sign change needed?
	forward(0) *= -1;
	forward(1) *= -1;
	forward(2) *= -1;
	//*******************************
   
	// Calculate the right side (x) vector
	cross = up.Cross(forward);

	// Rotation (model/attitude) Matrix
	//shouldn't it be in column-major order?
	// X Column
	mTransformMatrix(0,0) = cross(0);
	mTransformMatrix(0,1) = cross(1);
	mTransformMatrix(0,2) = cross(2);
	mTransformMatrix(0,3) = 0.0f;
         
	// Y Column
	mTransformMatrix(1,0) = up(0);
	mTransformMatrix(1,1) = up(1);
	mTransformMatrix(1,2) = up(2);
	mTransformMatrix(1,3) = 0.0f;       
                                    
	// Z Column
	mTransformMatrix(2,0) = forward(0);
	mTransformMatrix(2,1) = forward(1);
	mTransformMatrix(2,2) = forward(2);
	mTransformMatrix(2,3) = 0.0f;

	// Translation
	// cameras use the absolute (world) position to compute the transorm matrix used
	// to compute the view matrix.
	if (useAbsolutePosition)
	{
		mTransformMatrix(3,0) = mAbsolutePosition(0);
		mTransformMatrix(3,1) = mAbsolutePosition(1);
		mTransformMatrix(3,2) = mAbsolutePosition(2);
		mTransformMatrix(3,3) = 1.0f;
	}
	else
	{
		mTransformMatrix(3,0) = mRelativePosition(0);
		mTransformMatrix(3,1) = mRelativePosition(1);
		mTransformMatrix(3,2) = mRelativePosition(2);
		mTransformMatrix(3,3) = 1.0f;
	}
}

void SceneNode::UpdateState(bool useAbsolutePosition)
{
		
	//might be that the parent position/rotation has changed,in which case the children would not be aware of it.
	//update position
	/*Note: absolute position and rotation are not really needed for rendering, since they will be given
	  by the current matrix in the model stack, which is a multiplication of all the (relative) transform matrices
	  up to this point in the scene graph. 
	  They may or may not be needed for some special calculation / special effect in the future so this two lines will
	  remain here for now. They might be cut for efficiency, though. 
	*/
	mAbsolutePosition = mParent->GetAbsolutePosition() + mRelativePosition;
	//update rotation
	mToWorld = mToParent * mParent->GetWorldAttitude(); 
	//set the full transform for the renderer.
	if (!bUseLookAt)
		BuildTransformFromRotation(useAbsolutePosition);
	else
		BuildTransformFromLookAt(useAbsolutePosition);
}

void SceneNode::LoadUniforms(list<ShaderUniform> &uniforms, Material* material, RootNode* root)
{
	LightNode* activeLight;
	
	//******temporary
	//assume for the moment there is only one active light in the shader. 
	activeLight = root->GetActiveLights().front();
	//*******

	for (auto it = uniforms.begin(); it != uniforms.end(); ++it)
	{
		switch ((*it).Type())
		{
		case UNF_MVP_MATRIX:
			{
				Matrix4x4 MVP = root->GetCurrentMVPMatrix();
				(*it).SetValue(MVP);
				break;
			}
		case UNF_MV_MATRIX:
			{
				Matrix4x4 modelview = root->mModelViewStack.GetCurrentMatrix();
				(*it).SetValue(modelview);
				break;
			}
		case UNF_PROJ_MATRIX:
			{
				Matrix4x4 proj = root->mProjectionStack.GetCurrentMatrix();
				(*it).SetValue(proj);
				break;
			}
		case UNF_MODEL_MATRIX:
			{
				Matrix4x4 model = root->mModelViewStack.GetModelMatrix();
				(*it).SetValue(model);
				break;
			}
		case UNF_VIEW_MATRIX:
			{
				Matrix4x4 view = root->mModelViewStack.GetViewMatrix();
				(*it).SetValue(view);
				break;
			}
		case UNF_CAM_LOOKAT:
			{
				float gl_pos[4];
				memcpy(gl_pos, root->GetCameraTarget().GetComponents(), sizeof(float)*3);
				gl_pos[3] = 1.0;
				(*it).SetValue(gl_pos,4);
				break;
			}
		case UNF_COLOR:
			{
				(*it).SetValue(material->mColor,4);
				break;
			}
		case UNF_DIFFUSE_COLOR:
			{
				(*it).SetValue(material->mDiffuse,4);
				break;
			}
		case UNF_AMBIENT_COLOR:
			{
				(*it).SetValue(material->mAmbient,4);
				break;
			}
		case UNF_SPECULAR_COLOR:
			{
				(*it).SetValue(material->mSpecular,4);
				break;
			}
		case UNF_SHININESS:
			{
				(*it).SetValue(material->mShininess);
				break;
			}
		case UNF_LIGHT_POS:
			{
				//note that the light position is passed in in world coordinates!
				float gl_pos[4];
				memcpy(gl_pos, activeLight->GetAbsolutePosition().GetComponents(), sizeof(float)*3);
				activeLight->IsDirectional() ? gl_pos[3] = 1.0 : gl_pos[3] = 1.0;
				(*it).SetValue(gl_pos,4);
				break;
			}
		case UNF_LIGHT_DIR:
			{
				float gl_pos[4];
				memcpy(gl_pos, activeLight->GetLookAt().GetComponents(), sizeof(float)*3);
				gl_pos[3] = 1.0;
				(*it).SetValue(gl_pos,4);
				break;
			}
		case UNF_LIGHT_DIFFUSE:
			{
				(*it).SetValue(activeLight->GetDiffuseColor(),4);
				break;
			}
		case UNF_LIGHT_AMBIENT:
			{
				(*it).SetValue(activeLight->GetAmbientColor(),4);
				break;
			}
		case UNF_LIGHT_SPECULAR:
			{
				(*it).SetValue(activeLight->GetSpecularColor(),4);
				break;
			}
		case UNF_DIFFUSE_TEX0:
			{
				//bind the desired texture
				mRenderer->BindTexture(material->mDiffuseTextures[0]);
				//this is not the texture id, rather it represents the order in which the will be bound
				//i.e. the first bound texture has a uniform of 0, the second a uniform of 1 and so on, 
				//regardless of their actual gl texture id.
				//note that the current active texture has an id of depth - 1 since it's zero based.  
				int tex_stack_id = mRenderer->GetActiveTexStackDepth() - 1;
                (*it).SetValue( tex_stack_id );
				//this case should not be hit if there is no texture there, 
				//TODO: add log entry/debug info.
				break;
			}
		case UNF_DIFFUSE_TEX1:
			{
				mRenderer->BindTexture(material->mDiffuseTextures[1]);
				int tex_stack_id = mRenderer->GetActiveTexStackDepth();
				(*it).SetValue( tex_stack_id );
				break;
			}
		case UNF_DIFFUSE_TEX2:
			{
				mRenderer->BindTexture(material->mDiffuseTextures[2]);
				int tex_stack_id = mRenderer->GetActiveTexStackDepth();
				(*it).SetValue( tex_stack_id );
				break;
			}
		case UNF_SPECULAR_TEX0:
			{
				mRenderer->BindTexture(material->mSpecularTextures[0]);
				int tex_stack_id = mRenderer->GetActiveTexStackDepth(); 
				(*it).SetValue( tex_stack_id );
				break;
			}
		case UNF_NORMAL_MAP:
			{
				mRenderer->BindTexture(material->mBumpTextures[0]);
				int tex_stack_id = mRenderer->GetActiveTexStackDepth();
				(*it).SetValue( tex_stack_id );
				break;
			}
		case UNF_ALPHA_MAP:
			{
				mRenderer->BindTexture(material->mAlphaTextures[0]);
				int tex_stack_id = mRenderer->GetActiveTexStackDepth();
				(*it).SetValue( tex_stack_id );
				break;
			}
		case UNF_CUBE_MAP:
			{
				mRenderer->BindTexture(root->GetCurrentSkyMap());
				int tex_stack_id = mRenderer->GetActiveTexStackDepth();
				(*it).SetValue( tex_stack_id );
			}
			break;
		default:
			break;
		}
	}
}

void SceneNode::ScriptAnimation(string source, string funcName)
{
	ScriptManager* scriptManager = ScriptManager::GetInstance();

	scriptManager->RegisterLuaFunction(source, funcName);

	mLuaFuncName = funcName;
	bScriptAnimated = true;
}

void SceneNode::UpdateStateFromScript(float dt)
{
	ScriptManager *pScript = ScriptManager::GetInstance();
    
    LuaObject func_object = pScript->GetLuaFunction(mLuaFuncName);
	LuaFunction<LuaObject> GetState( func_object );

	LuaObject position;
	pScript->MakeTable(position);

	position.SetNumber(0, mRelativePosition.X());
	position.SetNumber(1, mRelativePosition.Y());
	position.SetNumber(2, mRelativePosition.Z());

	LuaObject state = GetState(position, dt);
		
	if (!state.IsTable())
		return;
		
    Vector3 out_pos(state[0].GetFloat(), state[1].GetFloat(), state[2].GetFloat());
    
	//update position
	SetRelativePosition( out_pos );
}


}

