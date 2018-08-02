/*

Mesh Node

*/

#ifndef MESHNODE_H
#define MESHNODE_H

#include "scene_node.h"
#include "material.h"
#include "mesh.h"

namespace NYX {

class mVertexBuffer;
class mIndexBuffer;
class VertexArrayObject;
class Effect;
    
class NYX_EXPORT MeshNode : public SceneNode
{
public:

    MeshNode(SceneNode *parent, IRenderer *renderer, std::string name, VertexBuffer* vb, IndexBuffer* ib, VertexArrayObject* vao);
    ~MeshNode();

	void AssignMesh(MeshPtr mesh) { mMesh = mesh; }
    
    void SetupVAO( Effect* program );
	virtual void ProcessNode();

protected:

    VertexBuffer* mVertexBuffer = nullptr;
    IndexBuffer* mIndexBuffer = nullptr;
    VertexArrayObject* mVertexArray = nullptr;
	MeshPtr mMesh;
};

}

#endif // MESHNODE_H
