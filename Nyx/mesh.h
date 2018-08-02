/*

Mesh

*/

#ifndef MESH_H
#define MESH_H

#include "irenderer.h"
#include "material.h"

namespace NYX {

class Model;
class MeshNode;

enum E_VERTEX_OFFSETS //in floats not bytes
{
	E_POSITION = 0,
	E_NORMAL = 3,
	E_TEXTURE = 6,
	E_TANGENT = 8,
	E_BITANGENT = 12
};

struct NYX_EXPORT Vertex
{
	Vertex() {
		//fixes VS complaing about uninitialized values
		memset(texCoord, 0, 2*sizeof(float));
		memset(position, 0, 3*sizeof(float));
		memset(normal, 0, 3*sizeof(float));
		memset(tangent, 0, 4*sizeof(float));
		memset(bitangent, 0, 3*sizeof(float));
	}

	float position[3];
	float normal[3];
	float texCoord[2];
	float tangent[4];
	float bitangent[3];
};

class NYX_EXPORT Mesh 
{
	friend class Model;
	friend class MeshNode;
	friend bool MeshCompFunc(const MeshPtr lhs, const MeshPtr rhs);

public:
	
	enum E_BUFFER_TYPE
	{
		E_VERTEX_BUFFER,
		E_INDEX_BUFFER,
		E_TEXTURE_BUFFER,
		E_NORMAL_BUFFER
	};

    Mesh(Model* parent);

    ~Mesh();

    void SetupVertexArray( void );
    void Initialize(uint startIndex, uint count, Material* mat,
                    IRenderer::E_POLYGON_TYPE ePolygon = IRenderer::E_TRIANGLE, int customPolySize = 0);
    
	uint PolyCount( void )                                            { return mPolyCount; }
	uint VertexCount( void )                                          { return mVertexCount; }
	uint StartIndex( void )                                           { return mStartIndex; }
	Model* GetParent( void )                                          { return mParent; }
	Material* GetMaterial(void)                                       { return mMaterial; }
	IRenderer::E_POLYGON_TYPE PolygonType( void )                     { return mPolygonType; }
	int CustomPolySize( void )                                        { return mCustomPolySize; }

private:

	Model* mParent;
	Material* mMaterial;
    
	uint mStartIndex;
	uint mPolyCount;
	uint mVertexCount;
	IRenderer::E_POLYGON_TYPE mPolygonType;
	int mCustomPolySize;
};

}

#endif //MESH_H