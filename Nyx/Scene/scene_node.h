/*

Generic Scene Node
 
*/

#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>

#include "log_manager.h"
#include "irenderer.h"
#include "Events/event_manager.h"
#include "Math/Matrix4x4.h"
#include "shader_uniform.h"

namespace NYX {

class RootNode;
class SceneNode;

typedef std::shared_ptr< SceneNode > SceneNodePtr;

enum eSceneNode
{
    SCENE_ROOT_NODE = 0x0010,
    SCENE_CAMERA_NODE = 0x0020,
    SCENE_LIGHT_NODE = 0x0030,
    SCENE_OBJECT_NODE = 0x0030,
};


class NYX_EXPORT SceneNode
{
protected:

	class NodeListener : public BaseEventListener
    {
	protected:
		SceneNode *mParent;

    public:
        NodeListener(string name, SceneNode *parent) : 
			BaseEventListener(name),
			mParent(parent)
		{}
		virtual ~NodeListener() {}

        virtual bool ProcessEvent(EventPtr event);
    };

	friend class NodeListener;

public:

    SceneNode(SceneNode *parent, IRenderer *renderer, std::string name);
    virtual ~SceneNode();

	std::string GetName();
	void SetName(std::string name);
	unsigned int GetID();
	eSceneNode GetType();
	
	Matrix3x3 GetWorldAttitude();
	Matrix3x3 GetLocalAttitude();
	Vector3 GetRelativePosition();
	Vector3 GetAbsolutePosition();
	//the full 4x4 transform matrix (rotation + translation) to be loaded as
	//the modelview matrix in the renderer.
	Matrix4x4& GetTransformMatrix();
	void SetLocalAttitude(Matrix3x3 rotation);
	void SetRelativePosition(Vector3 position);
	void SetWorldAttitude(Matrix3x3 rotation);
	void SetAbsolutePosition(Vector3 position);

	void SetDynamic(bool dynamic);
	bool IsDynamic();

	Vector3& GetLookAt();
	Vector3& GetUpVector();

	void LookAt(Vector3& target);
	void UpVector(Vector3& up);
	
	void AddChildNode(SceneNodePtr node);
	bool HasChildren();
	int GetChildrenNum();

	SceneNodePtr GetChildNodeByID(unsigned int nodeID);
	SceneNodePtr GetChildNodeByName(std::string nodeName);
	void RemoveChildNodeByID(unsigned int nodeID);
	void RemoveChildNodeByName(std::string nodeName);
	void ClearAllChildren();

	/*can't use a shared pointer to host the target.
	In the event that the target is also the parent (say, a camera following and looking at a mesh),
	neither child nor parent will ever be destroyed, because the parent holds a shared pointer to the child (obviously)
	while the child holds a pointer to the parent as target. Each will keep the other alive with a counter >= 1.
	The solution to prevent this is using a normal pointer.
	*/
	void SetTarget(SceneNode* target);
	void SetTransformToLookAt(bool useLookAt);
	bool TransformIsLookAt();

	RootNode* GetRootNode();

	virtual SceneNode* GetParentNode();

	virtual void ProcessNode() = 0;
	
	void ScriptAnimation(std::string source, std::string funcName);

protected:

	EventListenerPtr mEventListener;

	eSceneNode mNodeType;
	std::string mName;
	unsigned int mID;
	
	bool bIsDynamic;

	Matrix3x3 mToParent; //relative rotation
	Matrix3x3 mToWorld; //absolute (in world coordinates) rotation
	Vector3 mRelativePosition; 
	Vector3 mAbsolutePosition;
	
	/* Used to be only in the camera node, but they have been moved here instead because:
		a) the lights casting shadows need to compute the view matrix in the same way a camera would
		(and it's easier to point a camera/light with "look at" methods)
		b) some models/meshes need to be aligned with a target mesh and, again, it's easier to do it 
		with "look at" methods than trying to compute the correct set of rotations. 
	*/
	Vector3 mLookAt;
	Vector3 mUp;
	/*Sets whether the transform matrix should be built using the "look at" method or from 
	  a rotation matrix that's been set externally.
	  The former method is most commonly used for pointing cameras and lights (and all the meshes that
	  need to point to a target), while the latter for objects that are dynamically simulated, (i.e.
	  rotation and translation come from the physics engine). 
	  By default, it's set to false except for cameras and lights, but can be overridden.  
	*/
	bool bUseLookAt; 
	bool bHasTarget;

	Matrix4x4 mTransformMatrix; //full 4x4 transform (position + rotation)

	std::list<SceneNodePtr> mChildren;
	SceneNode *mParent;
	IRenderer *mRenderer;

	SceneNode* mTarget;

	void BuildTransformFromRotation(bool useAbsoluteCoords = false);
	void BuildTransformFromLookAt(bool useAbsolutePosition = false);

	virtual void UpdateState(bool useAbsolutePosition = false);
	void LoadUniforms(list<ShaderUniform> &uniforms, Material* material, RootNode* root);

	bool bScriptAnimated;
	std::string mLuaFuncName;

	void UpdateStateFromScript(float dt);
};


inline std::string SceneNode::GetName() { return mName; }
inline void SceneNode::SetName(std::string name) { mName = name; }
inline unsigned int SceneNode::GetID()  { return mID; }
inline eSceneNode SceneNode::GetType() { return  mNodeType; }
	
inline Matrix3x3 SceneNode::GetWorldAttitude() { return mToWorld; }
inline Matrix3x3 SceneNode::GetLocalAttitude() { return mToParent; }
inline Vector3 SceneNode::GetRelativePosition() { return mRelativePosition; }
inline Vector3 SceneNode::GetAbsolutePosition() { return mAbsolutePosition; }

inline void SceneNode::SetDynamic(bool dynamic) { bIsDynamic = dynamic; }
inline bool SceneNode::IsDynamic() { return bIsDynamic; }

inline Matrix4x4& SceneNode::GetTransformMatrix() { return mTransformMatrix; } 

inline void SceneNode::SetLocalAttitude(Matrix3x3 rotation) {
	mToParent = rotation;
	mToWorld = rotation * mParent->GetWorldAttitude();
}

inline void SceneNode::SetRelativePosition(Vector3 position) { 
	mRelativePosition = position;
	mAbsolutePosition = mParent->GetAbsolutePosition() + position;
}

inline void SceneNode::SetWorldAttitude(Matrix3x3 rotation)
{
	mToWorld = rotation;
	//need to add inverse method to Matrix3x3
	//mToParent = mToWorld * mParent->GetWorldAttitude().Inverse();
}

inline void SceneNode::SetAbsolutePosition(Vector3 position)
{
	mAbsolutePosition = position;
	mRelativePosition = mAbsolutePosition - mParent->GetAbsolutePosition();
}

inline Vector3& SceneNode::GetLookAt() {return mLookAt;}
inline Vector3& SceneNode::GetUpVector() {return mUp;}

inline void SceneNode::LookAt(Vector3& target) {mLookAt = target;}
inline void SceneNode::UpVector(Vector3& up) {mUp = up;}
	
inline void SceneNode::AddChildNode(SceneNodePtr node) { mChildren.push_back(node); }
inline bool SceneNode::HasChildren() { return !mChildren.empty(); }
inline int SceneNode::GetChildrenNum() { return mChildren.size(); }

inline SceneNodePtr SceneNode::GetChildNodeByID(unsigned int nodeID) {
	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		if ((*it)->GetID() == nodeID)
			return (*it);

	return SceneNodePtr();
}

inline SceneNodePtr SceneNode::GetChildNodeByName(std::string nodeName) {
	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		if ((*it)->GetName() == nodeName)
			return (*it);
		
	return SceneNodePtr();
}

inline void SceneNode::RemoveChildNodeByID(unsigned int nodeID) {
	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		if ((*it)->GetID() == nodeID)
		{
			(*it).reset();
			mChildren.erase(it);
		} 
}

inline void SceneNode::RemoveChildNodeByName(std::string nodeName) {
	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		if ((*it)->GetName() == nodeName)
		{
			(*it).reset();
			mChildren.erase(it);
		}
}

inline void SceneNode::ClearAllChildren() { 
	for (std::list<SceneNodePtr>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		(*it).reset();
	mChildren.clear(); 
}

inline void SceneNode::SetTarget(SceneNode* target) { 
	mTarget = target;
	bHasTarget = true;
	bUseLookAt = true;
}

inline void SceneNode::SetTransformToLookAt(bool useLookAt) { bUseLookAt = useLookAt; }
inline bool SceneNode::TransformIsLookAt() { return bUseLookAt; }

inline SceneNode* SceneNode::GetParentNode() { return mParent; }

}

#endif // SCENENODE_H
