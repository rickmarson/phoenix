/*

3D Model Node

*/

#ifndef MODELNODE_H
#define MODELNODE_H

#include "scene_node.h"
#include "model.h"

namespace NYX {

// Required Classes
class VertexBuffer;
class IndexBuffer;
class VertexArrayObject;
    
class NYX_EXPORT ModelNode : public SceneNode
{
	class ModelListener : public SceneNode::NodeListener
    {
    public:
        ModelListener(string name, SceneNode *parent) : 
			SceneNode::NodeListener(name, parent)
		{}
		virtual ~ModelListener() {}

        virtual bool ProcessEvent(EventPtr event);
    };

	friend class ModelListener;

public:

    ModelNode(SceneNode *parent, IRenderer *renderer, std::string name);
    ~ModelNode();

	void AssignModel(std::string modelName);
    void SetScaleFactor( float scale );

	virtual void ProcessNode();

protected:

	void BuildChildren();

	ModelPtr mModel;
    float scale_factor = 0.0f;
    VertexBuffer* mVertexBuffer = nullptr;
    IndexBuffer* mIndexBuffer = nullptr;
    VertexArrayObject* mVertexArrayObject = nullptr;
};

}

#endif // MODELNODE_H
