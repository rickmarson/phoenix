/*

Mesh

*/

#include "mesh.h"

namespace NYX {

    Mesh::Mesh(Model* parent) :
        mParent(parent),
        mPolyCount(0),
        mPolygonType(IRenderer::E_TRIANGLE)
	{
    }

    Mesh::~Mesh()
    {
        mParent = NULL;
    }
	
    void Mesh::Initialize(uint startIndex, uint count, Material* mat, IRenderer::E_POLYGON_TYPE ePolygon, int customPolySize)
	{
		//not yet supported by model.
		mPolygonType = ePolygon;
		mCustomPolySize = customPolySize;
		//***************

		mStartIndex = startIndex;
		mPolyCount = count;
		mMaterial = mat;

		switch (ePolygon)
		{
		case IRenderer::E_POINT:
			mVertexCount = count;
			break;
		case IRenderer::E_LINE:
			mVertexCount = count*2;
			break;
		case IRenderer::E_TRIANGLE:
			mVertexCount = count*3;
			break;
		case IRenderer::E_QUAD:
			mVertexCount = count*4;
			break;
		case IRenderer::E_POLYGON:
			mVertexCount = count*customPolySize;
			break;
		default:
			break;
		}
	}


}
