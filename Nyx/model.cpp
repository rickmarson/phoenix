//-----------------------------------------------------------------------------
// Copyright (c) 2007 dhpoware. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
//
// The methods normalize() and scale() are based on source code from
// http://www.mvps.org/directx/articles/scalemesh9.htm.
//
// The addVertex() method is based on source code from the Direct3D MeshFromOBJ
// sample found in the DirectX SDK.
//
// The generateTangents() method is based on public source code from
// http://www.terathon.com/code/tangent.php.
//
// The importGeometryFirstPass(), importGeometrySecondPass(), and
// importMaterials() methods are based on source code from Nate Robins' OpenGL
// Tutors programs (http://www.xmission.com/~nate/tutors.html).
//
//-----------------------------------------------------------------------------

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

#include "model.h"
#include "Cache/resource_cache.h"
#include "physfs.h"

// added to support reading from a memory stream
#include <iostream>
#include <sstream>
// ---------------------------------------------

//-----------------------------------------------------
using namespace std;
//-----------------------------------------------------

namespace NYX {

bool MeshCompFunc(const MeshPtr lhs, const MeshPtr rhs)
{
	return lhs->mMaterial->Alpha() > rhs->mMaterial->Alpha();
}

Model::Model()
{
    m_hasPositions = false;
    m_hasNormals = false;
    m_hasTextureCoords = false;
    m_hasTangents = false;

    m_numberOfVertexCoords = 0;
    m_numberOfTextureCoords = 0;
    m_numberOfNormals = 0;
    m_numberOfTriangles = 0;
    m_numberOfMaterials = 0;
    m_numberOfMeshes = 0;

    m_center[0] = m_center[1] = m_center[2] = 0.0;
    m_width = m_height = m_length = m_radius = 0.0;
}

Model::~Model()
{
    destroy();
}

void Model::bounds(float center[3], float &width, float &height,
                      float &length, float &radius) const
{
    float xMax = std::numeric_limits<float>::min();
    float yMax = std::numeric_limits<float>::min();
    float zMax = std::numeric_limits<float>::min();

    float xMin = std::numeric_limits<float>::max();
    float yMin = std::numeric_limits<float>::max();
    float zMin = std::numeric_limits<float>::max();

    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    int numVerts = static_cast<int>(m_vertexBuffer.size());

    for (int i = 0; i < numVerts; ++i)
    {
        x = m_vertexBuffer[i].position[0];
        y = m_vertexBuffer[i].position[1];
        z = m_vertexBuffer[i].position[2];

        if (x < xMin)
            xMin = x;

        if (x > xMax)
            xMax = x;

        if (y < yMin)
            yMin = y;

        if (y > yMax)
            yMax = y;

        if (z < zMin)
            zMin = z;

        if (z > zMax)
            zMax = z;
    }

    center[0] = (xMin + xMax) / 2.0;
    center[1] = (yMin + yMax) / 2.0;
    center[2] = (zMin + zMax) / 2.0;

    width = xMax - xMin;
    height = yMax - yMin;
    length = zMax - zMin;

    radius = std::max(std::max(width, height), length);
}

void Model::destroy()
{
    m_hasPositions = false;
    m_hasTextureCoords = false;
    m_hasNormals = false;
    m_hasTangents = false;

    m_numberOfVertexCoords = 0;
    m_numberOfTextureCoords = 0;
    m_numberOfNormals = 0;
    m_numberOfTriangles = 0;
    m_numberOfMaterials = 0;
    m_numberOfMeshes = 0;

    m_center[0] = m_center[1] = m_center[2] = 0.0;
    m_width = m_height = m_length = m_radius = 0.0;

    m_directoryPath.clear();

    m_meshes.clear();
    m_materials.clear();
    m_vertexBuffer.clear();
    m_indexBuffer.clear();
    m_attributeBuffer.clear();

    m_vertexCoords.clear();
    m_textureCoords.clear();
    m_normals.clear();

    m_materialCache.clear();
    m_vertexCache.clear();
}

bool Model::importFromMemory(const char *stream, bool rebuildNormals)
{
    // Import the OBJ file.

    importGeometryFirstPass(stream);
    importGeometrySecondPass(stream);

    // Perform post import tasks.

    buildMeshes();
    bounds(m_center, m_width, m_height, m_length, m_radius);

    // Build vertex normals if required.

    if (rebuildNormals)
    {
        generateNormals();
    }
    else
    {
        if (!hasNormals())
            generateNormals();
    }

    // Build tangents is required.

    for (int i = 0; i < m_numberOfMaterials; ++i)
    {
        if (!m_materials[i].mBumpTextures.empty())
        {
            generateTangents();
            break;
        }
    }

    return true;
}

void Model::normalize(float scaleTo, bool center)
{
    float width = 0.0;
    float height = 0.0;
    float length = 0.0;
    float radius = 0.0;
    float centerPos[3] = {0.0};

    bounds(centerPos, width, height, length, radius);

    float scalingFactor = scaleTo / radius;
    float offset[3] = {0.0};

    if (center)
    {
        offset[0] = -centerPos[0];
        offset[1] = -centerPos[1];
        offset[2] = -centerPos[2];
    }
    else
    {
        offset[0] = 0.0;
        offset[1] = 0.0;
        offset[2] = 0.0;
    }

    scale(scalingFactor, offset);
    bounds(m_center, m_width, m_height, m_length, m_radius);
}

void Model::reverseWinding()
{
    int swap = 0;

    // Reverse face winding.
    for (int i = 0; i < static_cast<int>(m_indexBuffer.size()); i += 3)
    {
        swap = m_indexBuffer[i + 1];
        m_indexBuffer[i + 1] = m_indexBuffer[i + 2];
        m_indexBuffer[i + 2] = swap;
    }

    float *pNormal = 0;
    float *pTangent = 0;

    // Invert normals and tangents.
    for (int i = 0; i < static_cast<int>(m_vertexBuffer.size()); ++i)
    {
        pNormal = m_vertexBuffer[i].normal;
        pNormal[0] = -pNormal[0];
        pNormal[1] = -pNormal[1];
        pNormal[2] = -pNormal[2];

        pTangent = m_vertexBuffer[i].tangent;
        pTangent[0] = -pTangent[0];
        pTangent[1] = -pTangent[1];
        pTangent[2] = -pTangent[2];
    }
}

void Model::scale(float scaleFactor, float offset[3])
{
    float *pPosition = 0;

    for (int i = 0; i < static_cast<int>(m_vertexBuffer.size()); ++i)
    {
        pPosition = m_vertexBuffer[i].position;

        pPosition[0] += offset[0];
        pPosition[1] += offset[1];
        pPosition[2] += offset[2];

        pPosition[0] *= scaleFactor;
        pPosition[1] *= scaleFactor;
        pPosition[2] *= scaleFactor;
    }
}

void Model::addTrianglePos(int index, int material, int v0, int v1, int v2)
{
    //modified by R.Marson to avoid compile warnings
    Vertex vertex;

    m_attributeBuffer[index] = material;

    vertex.position[0] = m_vertexCoords[v0 * 3];
    vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
    m_indexBuffer[index * 3] = addVertex(v0, &vertex);

    vertex.position[0] = m_vertexCoords[v1 * 3];
    vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
    m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

    vertex.position[0] = m_vertexCoords[v2 * 3];
    vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
    m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
}

void Model::addTrianglePosNormal(int index, int material, int v0, int v1,
                                    int v2, int vn0, int vn1, int vn2)
{
    //modified by R.Marson to avoid compile warnings
    Vertex vertex;

    m_attributeBuffer[index] = material;

    vertex.position[0] = m_vertexCoords[v0 * 3];
    vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
    vertex.normal[0] = m_normals[vn0 * 3];
    vertex.normal[1] = m_normals[vn0 * 3 + 1];
    vertex.normal[2] = m_normals[vn0 * 3 + 2];
    m_indexBuffer[index * 3] = addVertex(v0, &vertex);

    vertex.position[0] = m_vertexCoords[v1 * 3];
    vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
    vertex.normal[0] = m_normals[vn1 * 3];
    vertex.normal[1] = m_normals[vn1 * 3 + 1];
    vertex.normal[2] = m_normals[vn1 * 3 + 2];
    m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

    vertex.position[0] = m_vertexCoords[v2 * 3];
    vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
    vertex.normal[0] = m_normals[vn2 * 3];
    vertex.normal[1] = m_normals[vn2 * 3 + 1];
    vertex.normal[2] = m_normals[vn2 * 3 + 2];
    m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
}

void Model::addTrianglePosTexCoord(int index, int material, int v0, int v1,
                                      int v2, int vt0, int vt1, int vt2)
{
    //modified by R.Marson to avoid compile warnings
    Vertex vertex;

    m_attributeBuffer[index] = material;

    vertex.position[0] = m_vertexCoords[v0 * 3];
    vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
    vertex.texCoord[0] = m_textureCoords[vt0 * 2];
    vertex.texCoord[1] = m_textureCoords[vt0 * 2 + 1];
    m_indexBuffer[index * 3] = addVertex(v0, &vertex);

    vertex.position[0] = m_vertexCoords[v1 * 3];
    vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
    vertex.texCoord[0] = m_textureCoords[vt1 * 2];
    vertex.texCoord[1] = m_textureCoords[vt1 * 2 + 1];
    m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

    vertex.position[0] = m_vertexCoords[v2 * 3];
    vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
    vertex.texCoord[0] = m_textureCoords[vt2 * 2];
    vertex.texCoord[1] = m_textureCoords[vt2 * 2 + 1];
    m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
}

void Model::addTrianglePosTexCoordNormal(int index, int material, int v0,
                                            int v1, int v2, int vt0, int vt1,
                                            int vt2, int vn0, int vn1, int vn2)
{
    //modified by R.Marson to avoid compile warnings
    Vertex vertex;

    m_attributeBuffer[index] = material;

    vertex.position[0] = m_vertexCoords[v0 * 3];
    vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
    vertex.texCoord[0] = m_textureCoords[vt0 * 2];
    vertex.texCoord[1] = m_textureCoords[vt0 * 2 + 1];
    vertex.normal[0] = m_normals[vn0 * 3];
    vertex.normal[1] = m_normals[vn0 * 3 + 1];
    vertex.normal[2] = m_normals[vn0 * 3 + 2];
    m_indexBuffer[index * 3] = addVertex(v0, &vertex);

    vertex.position[0] = m_vertexCoords[v1 * 3];
    vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
    vertex.texCoord[0] = m_textureCoords[vt1 * 2];
    vertex.texCoord[1] = m_textureCoords[vt1 * 2 + 1];
    vertex.normal[0] = m_normals[vn1 * 3];
    vertex.normal[1] = m_normals[vn1 * 3 + 1];
    vertex.normal[2] = m_normals[vn1 * 3 + 2];
    m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

    vertex.position[0] = m_vertexCoords[v2 * 3];
    vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
    vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
    vertex.texCoord[0] = m_textureCoords[vt2 * 2];
    vertex.texCoord[1] = m_textureCoords[vt2 * 2 + 1];
    vertex.normal[0] = m_normals[vn2 * 3];
    vertex.normal[1] = m_normals[vn2 * 3 + 1];
    vertex.normal[2] = m_normals[vn2 * 3 + 2];
    m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
}

int Model::addVertex(int hash, const Vertex *pVertex)
{
    int index = -1;
    std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

    if (iter == m_vertexCache.end())
    {
        // Vertex hash doesn't exist in the cache.

        index = static_cast<int>(m_vertexBuffer.size());
        m_vertexBuffer.push_back(*pVertex);
        m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
    }
    else
    {
        // One or more vertices have been hashed to this entry in the cache.

        const std::vector<int> &vertices = iter->second;
        const Vertex *pCachedVertex = 0;
        bool found = false;

        for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i)
        {
            index = *i;
            pCachedVertex = &m_vertexBuffer[index];

            if (memcmp(pCachedVertex, pVertex, sizeof(Vertex)) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            index = static_cast<int>(m_vertexBuffer.size());
            m_vertexBuffer.push_back(*pVertex);
            m_vertexCache[hash].push_back(index);
        }
    }

    return index;
}

void Model::buildMeshes()
{
    // Group the model's triangles based on material type
	MeshPtr pMesh;

    int materialId = -1;
    int numMeshes = 0;

    // Count the number of meshes.
    for (int i = 0; i < static_cast<int>(m_attributeBuffer.size()); ++i)
    {
        if (m_attributeBuffer[i] != materialId)
        {
            materialId = m_attributeBuffer[i];
            ++numMeshes;
        }
    }

    // Allocate memory for the meshes and reset counters.
    m_numberOfMeshes = numMeshes;
    m_meshes.resize(m_numberOfMeshes);
    numMeshes = 0;
    materialId = -1;

    // Build the meshes. One mesh for each unique material.
    for (int i = 0; i < static_cast<int>(m_attributeBuffer.size()); ++i)
    {
        if (m_attributeBuffer[i] != materialId)
        {
			MeshPtr newMesh(new Mesh(this));
            materialId = m_attributeBuffer[i];                   
			newMesh->mPolygonType = IRenderer::E_TRIANGLE;
            newMesh->mMaterial = &m_materials[materialId];
            newMesh->mStartIndex = i * 3;
            ++newMesh->mPolyCount;
			m_meshes[numMeshes++] = newMesh;
			pMesh = newMesh;
        }
        else
        {
            ++pMesh->mPolyCount;
        }
    }

    // Sort the meshes based on its material alpha. Fully opaque meshes
    // towards the front and fully transparent towards the back.
    std::sort(m_meshes.begin(), m_meshes.end(), MeshCompFunc);
}

void Model::generateNormals()
{
    const int *pTriangle = 0;
    Vertex *pVertex0 = 0;
    Vertex *pVertex1 = 0;
    Vertex *pVertex2 = 0;
    float edge1[3] = {0.0, 0.0, 0.0};
    float edge2[3] = {0.0, 0.0, 0.0};
    float normal[3] = {0.0, 0.0, 0.0};
    float length = 0.0;
    int totalVertices = getNumberOfVertices();
    int totalTriangles = getNumberOfTriangles();

    // Initialize all the vertex normals.
    for (int i = 0; i < totalVertices; ++i)
    {
        pVertex0 = &m_vertexBuffer[i];
        pVertex0->normal[0] = 0.0;
        pVertex0->normal[1] = 0.0;
        pVertex0->normal[2] = 0.0;
    }

    // Calculate the vertex normals.
    for (int i = 0; i < totalTriangles; ++i)
    {
        pTriangle = &m_indexBuffer[i * 3];

        pVertex0 = &m_vertexBuffer[pTriangle[0]];
        pVertex1 = &m_vertexBuffer[pTriangle[1]];
        pVertex2 = &m_vertexBuffer[pTriangle[2]];

        // Calculate triangle face normal.

        edge1[0] = pVertex1->position[0] - pVertex0->position[0];
        edge1[1] = pVertex1->position[1] - pVertex0->position[1];
        edge1[2] = pVertex1->position[2] - pVertex0->position[2];

        edge2[0] = pVertex2->position[0] - pVertex0->position[0];
        edge2[1] = pVertex2->position[1] - pVertex0->position[1];
        edge2[2] = pVertex2->position[2] - pVertex0->position[2];

        normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
        normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
        normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

        // Accumulate the normals.

        pVertex0->normal[0] += normal[0];
        pVertex0->normal[1] += normal[1];
        pVertex0->normal[2] += normal[2];

        pVertex1->normal[0] += normal[0];
        pVertex1->normal[1] += normal[1];
        pVertex1->normal[2] += normal[2];

        pVertex2->normal[0] += normal[0];
        pVertex2->normal[1] += normal[1];
        pVertex2->normal[2] += normal[2];
    }

    // Normalize the vertex normals.
    for (int i = 0; i < totalVertices; ++i)
    {
        pVertex0 = &m_vertexBuffer[i];

        length = 1.0f / sqrtf(pVertex0->normal[0] * pVertex0->normal[0] +
            pVertex0->normal[1] * pVertex0->normal[1] +
            pVertex0->normal[2] * pVertex0->normal[2]);

        pVertex0->normal[0] *= length;
        pVertex0->normal[1] *= length;
        pVertex0->normal[2] *= length;
    }

    m_hasNormals = true;
}

void Model::generateTangents()
{
    const int *pTriangle = 0;
    Vertex *pVertex0 = 0;
    Vertex *pVertex1 = 0;
    Vertex *pVertex2 = 0;
    float edge1[3] = {0.0, 0.0, 0.0};
    float edge2[3] = {0.0, 0.0, 0.0};
    float texEdge1[2] = {0.0, 0.0};
    float texEdge2[2] = {0.0, 0.0};
    float tangent[3] = {0.0, 0.0, 0.0};
    float bitangent[3] = {0.0, 0.0, 0.0};
    float det = 0.0;
    float nDotT = 0.0;
    float bDotB = 0.0;
    float length = 0.0;
    int totalVertices = getNumberOfVertices();
    int totalTriangles = getNumberOfTriangles();

    // Initialize all the vertex tangents and bitangents.
    for (int i = 0; i < totalVertices; ++i)
    {
        pVertex0 = &m_vertexBuffer[i];

        pVertex0->tangent[0] = 0.0;
        pVertex0->tangent[1] = 0.0;
        pVertex0->tangent[2] = 0.0;
        pVertex0->tangent[3] = 0.0;

        pVertex0->bitangent[0] = 0.0;
        pVertex0->bitangent[1] = 0.0;
        pVertex0->bitangent[2] = 0.0;
    }

    // Calculate the vertex tangents and bitangents.
    for (int i = 0; i < totalTriangles; ++i)
    {
        pTriangle = &m_indexBuffer[i * 3];

        pVertex0 = &m_vertexBuffer[pTriangle[0]];
        pVertex1 = &m_vertexBuffer[pTriangle[1]];
        pVertex2 = &m_vertexBuffer[pTriangle[2]];

        // Calculate the triangle face tangent and bitangent.

        edge1[0] = pVertex1->position[0] - pVertex0->position[0];
        edge1[1] = pVertex1->position[1] - pVertex0->position[1];
        edge1[2] = pVertex1->position[2] - pVertex0->position[2];

        edge2[0] = pVertex2->position[0] - pVertex0->position[0];
        edge2[1] = pVertex2->position[1] - pVertex0->position[1];
        edge2[2] = pVertex2->position[2] - pVertex0->position[2];

        texEdge1[0] = pVertex1->texCoord[0] - pVertex0->texCoord[0];
        texEdge1[1] = pVertex1->texCoord[1] - pVertex0->texCoord[1];

        texEdge2[0] = pVertex2->texCoord[0] - pVertex0->texCoord[0];
        texEdge2[1] = pVertex2->texCoord[1] - pVertex0->texCoord[1];

        det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

        if (fabs(det) < 1e-6)
        {
            tangent[0] = 1.0;
            tangent[1] = 0.0;
            tangent[2] = 0.0;

            bitangent[0] = 0.0;
            bitangent[1] = 1.0;
            bitangent[2] = 0.0;
        }
        else
        {
            det = 1.0 / det;

            tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
            tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
            tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

            bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
            bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
            bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
        }

        // Accumulate the tangents and bitangents.

        pVertex0->tangent[0] += tangent[0];
        pVertex0->tangent[1] += tangent[1];
        pVertex0->tangent[2] += tangent[2];
        pVertex0->bitangent[0] += bitangent[0];
        pVertex0->bitangent[1] += bitangent[1];
        pVertex0->bitangent[2] += bitangent[2];

        pVertex1->tangent[0] += tangent[0];
        pVertex1->tangent[1] += tangent[1];
        pVertex1->tangent[2] += tangent[2];
        pVertex1->bitangent[0] += bitangent[0];
        pVertex1->bitangent[1] += bitangent[1];
        pVertex1->bitangent[2] += bitangent[2];

        pVertex2->tangent[0] += tangent[0];
        pVertex2->tangent[1] += tangent[1];
        pVertex2->tangent[2] += tangent[2];
        pVertex2->bitangent[0] += bitangent[0];
        pVertex2->bitangent[1] += bitangent[1];
        pVertex2->bitangent[2] += bitangent[2];
    }

    // Orthogonalize and normalize the vertex tangents.
    for (int i = 0; i < totalVertices; ++i)
    {
        pVertex0 = &m_vertexBuffer[i];

        // Gram-Schmidt orthogonalize tangent with normal.

        nDotT = pVertex0->normal[0] * pVertex0->tangent[0] +
                pVertex0->normal[1] * pVertex0->tangent[1] +
                pVertex0->normal[2] * pVertex0->tangent[2];

        pVertex0->tangent[0] -= pVertex0->normal[0] * nDotT;
        pVertex0->tangent[1] -= pVertex0->normal[1] * nDotT;
        pVertex0->tangent[2] -= pVertex0->normal[2] * nDotT;

        // Normalize the tangent.

        length = 1.0f / sqrtf(pVertex0->tangent[0] * pVertex0->tangent[0] +
                              pVertex0->tangent[1] * pVertex0->tangent[1] +
                              pVertex0->tangent[2] * pVertex0->tangent[2]);

        pVertex0->tangent[0] *= length;
        pVertex0->tangent[1] *= length;
        pVertex0->tangent[2] *= length;

        // Calculate the handedness of the local tangent space.
        // The bitangent vector is the cross product between the triangle face
        // normal vector and the calculated tangent vector. The resulting
        // bitangent vector should be the same as the bitangent vector
        // calculated from the set of linear equations above. If they point in
        // different directions then we need to invert the cross product
        // calculated bitangent vector. We store this scalar multiplier in the
        // tangent vector's 'w' component so that the correct bitangent vector
        // can be generated in the normal mapping shader's vertex shader.
        //
        // Normal maps have a left handed coordinate system with the origin
        // located at the top left of the normal map texture. The x coordinates
        // run horizontally from left to right. The y coordinates run
        // vertically from top to bottom. The z coordinates run out of the
        // normal map texture towards the viewer. Our handedness calculations
        // must take this fact into account as well so that the normal mapping
        // shader's vertex shader will generate the correct bitangent vectors.
        // Some normal map authoring tools such as Crazybump
        // (http://www.crazybump.com/) includes options to allow you to control
        // the orientation of the normal map normal's y-axis.

        bitangent[0] = (pVertex0->normal[1] * pVertex0->tangent[2]) - 
                       (pVertex0->normal[2] * pVertex0->tangent[1]);
        bitangent[1] = (pVertex0->normal[2] * pVertex0->tangent[0]) -
                       (pVertex0->normal[0] * pVertex0->tangent[2]);
        bitangent[2] = (pVertex0->normal[0] * pVertex0->tangent[1]) - 
                       (pVertex0->normal[1] * pVertex0->tangent[0]);

        bDotB = bitangent[0] * pVertex0->bitangent[0] + 
                bitangent[1] * pVertex0->bitangent[1] + 
                bitangent[2] * pVertex0->bitangent[2];

        pVertex0->tangent[3] = (bDotB < 0.0f) ? 1.0 : -1.0;

        pVertex0->bitangent[0] = bitangent[0];
        pVertex0->bitangent[1] = bitangent[1];
        pVertex0->bitangent[2] = bitangent[2];
    }

    m_hasTangents = true;
}

void Model::importGeometryFirstPass(const char *stream)
{
	//Note that EOL must be unix only for now. 
    m_hasTextureCoords = false;
    m_hasNormals = false;

    m_numberOfVertexCoords = 0;
    m_numberOfTextureCoords = 0;
    m_numberOfNormals = 0;
    m_numberOfTriangles = 0;

    int v = 0;
    int vt = 0;
    int vn = 0;
    char buffer[256] = {0};
    string name;

	//line endings are controlled by perforce and are based on the local system,
	//so we can safely assume that under windows all input files will have windows EOL
	//and under Unix will have Unix EOL
#ifdef __WIN32__
	char new_line = '\r';
#else
	char new_line = '\n';
#endif

    // on osx, assigning a const char* to the stringstream using the assignement operator << randomly crashes on assignement. 
    // reason not yet known.
#if defined (__MACOSX__)
    string tmp(stream);
    stringstream memstream(tmp, ios::in | ios::out);
#else
    stringstream memstream( stringstream::in | stringstream::out );
    memstream << stream;
#endif
    
    while ( memstream.good() )
    {
    	memstream >> buffer;

        switch (buffer[0])
        {
        case 'f':   // v, v//vn, v/vt, v/vt/vn.
        	memstream >> buffer;

            if (strstr(buffer, "//")) // v//vn
            {
            	sscanf(buffer, "%d//%d", &v, &vn);
            	memstream >> buffer;
            	sscanf(buffer, "%d//%d", &v, &vn);
            	memstream >> buffer;
            	sscanf(buffer, "%d//%d", &v, &vn);
                ++m_numberOfTriangles;

                while (memstream.peek() != new_line)
                {
                	memstream >> buffer;
                	++m_numberOfTriangles;
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
                	memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}

            }
            else if (sscanf(buffer, "%d/%d/%d", &v, &vt, &vn) == 3) // v/vt/vn
            {
            	memstream >> buffer;
                sscanf(buffer, "%d/%d/%d", &v, &vt, &vn);
                memstream >> buffer;
                sscanf(buffer, "%d/%d/%d", &v, &vt, &vn);
                ++m_numberOfTriangles;

                while ( (memstream.peek() != new_line) ) 
                {
                	memstream >> buffer;
                	++m_numberOfTriangles;
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
                	memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}

            }
            else if (sscanf(buffer, "%d/%d", &v, &vt) == 2) // v/vt
            {
            	memstream >> buffer;
                sscanf(buffer, "%d/%d", &v, &vt);
                memstream >> buffer;
                sscanf(buffer, "%d/%d", &v, &vt);
                ++m_numberOfTriangles;

                while (memstream.peek() != new_line)
                {
                	memstream >> buffer;
                	++m_numberOfTriangles;
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
                	memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
            }
            else // v
            {
            	memstream >> buffer;
                sscanf(buffer, "%d", &v);
                memstream >> buffer;
                sscanf(buffer, "%d", &v);
                ++m_numberOfTriangles;

                while (memstream.peek() != new_line)
                {
                	memstream >> buffer;
                	++m_numberOfTriangles;
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
                	memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}

            }
            break;

        case 'm':   // mtllib
        	memstream >> buffer;
            importMaterialsFromMemory(buffer);

            if(memstream.peek() == new_line)
			{
				//throw away new line char
                memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
            break;

        case 'v':   // v, vt, or vn
            switch (buffer[1])
            {
            case '\0':
                memstream >> buffer >> buffer >> buffer;
                if(memstream.peek() == new_line)
				{
					//throw away new line char
                	memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
                ++m_numberOfVertexCoords;
                break;

            case 'n':
            	memstream >> buffer >> buffer >> buffer;
            	if(memstream.peek() == new_line)
				{
					//throw away new line char
                	memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
                ++m_numberOfNormals;
                break;

            case 't':
            	memstream >> buffer >> buffer;
            	if(memstream.peek() == new_line)
				{
					//throw away new line char
                	memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
                ++m_numberOfTextureCoords;

            default:
                break;
            }
            break;

        default:
			memstream.getline(buffer, 256);
        	if(memstream.peek() == new_line)
			{
				//throw away new line char
                memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
            break;
        }
    }

    m_hasPositions = m_numberOfVertexCoords > 0;
    m_hasNormals = m_numberOfNormals > 0;
    m_hasTextureCoords = m_numberOfTextureCoords > 0;

    // Allocate memory for the OBJ model data.
    m_vertexCoords.resize(m_numberOfVertexCoords * 3);
    m_textureCoords.resize(m_numberOfTextureCoords * 2);
    m_normals.resize(m_numberOfNormals * 3);
    m_indexBuffer.resize(m_numberOfTriangles * 3);
    m_attributeBuffer.resize(m_numberOfTriangles);

    // Define a default material if no materials were loaded.
    if (m_numberOfMaterials == 0)
    {
        //modified by R.Marson
        Material defaultMaterial;

        m_materials.push_back(defaultMaterial);
        m_materialCache[defaultMaterial.mName] = 0;
    }

    memstream.clear();
}

void Model::importGeometrySecondPass(const char *stream)
{
    int v[3] = {0};
    int vt[3] = {0};
    int vn[3] = {0};
    int numVertices = 0;
    int numTexCoords = 0;
    int numNormals = 0;
    int numTriangles = 0;
    int activeMaterial = 0;
    char buffer[256] = {0};
    std::string name;
    std::map<std::string, int>::const_iterator iter;

	//line endings are controlled by perforce and are based on the local system,
	//so we can safely assume that under windows all input files will have windows EOL
	//and under Unix will have Unix EOL
#ifdef __WIN32__
	char new_line = '\r';
#else
	char new_line = '\n';
#endif

	 // on osx, assigning a const char* to the stringstream using the assignement operator << randomly crashes on assignement. 
    // reason not yet known.
#if defined (__MACOSX__)
    string tmp(stream);
    stringstream memstream(tmp, ios::in | ios::out);
#else
    stringstream memstream( stringstream::in | stringstream::out );
    memstream << stream;
#endif

    while ( memstream.good() )
    {
    	memstream >> buffer;

        switch (buffer[0])
        {
        case 'f': // v, v//vn, v/vt, or v/vt/vn.
            v[0]  = v[1]  = v[2]  = 0;
            vt[0] = vt[1] = vt[2] = 0;
            vn[0] = vn[1] = vn[2] = 0;

            memstream >> buffer;

            if (strstr(buffer, "//")) // v//vn
            {
                sscanf(buffer, "%d//%d", &v[0], &vn[0]);
                memstream >> buffer;
                sscanf(buffer, "%d//%d", &v[1], &vn[1]);
                memstream >> buffer;
                sscanf(buffer, "%d//%d", &v[2], &vn[2]);

                v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
                v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

                vn[0] = (vn[0] < 0) ? vn[0] + numNormals - 1 : vn[0] - 1;
                vn[1] = (vn[1] < 0) ? vn[1] + numNormals - 1 : vn[1] - 1;
                vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

                addTrianglePosNormal(numTriangles++, activeMaterial,
                    v[0], v[1], v[2], vn[0], vn[1], vn[2]);

                v[1] = v[2];
                vn[1] = vn[2];

                while (memstream.peek() != new_line)
                {
                	memstream >> buffer;
                	sscanf(buffer, "%d//%d", &v[2], &vn[2]);

                    v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
                    vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

                    addTrianglePosNormal(numTriangles++, activeMaterial,
                        v[0], v[1], v[2], vn[0], vn[1], vn[2]);

                    v[1] = v[2];
                    vn[1] = vn[2];
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
            }
            else if (sscanf(buffer, "%d/%d/%d", &v[0], &vt[0], &vn[0]) == 3) // v/vt/vn
            {
            	memstream >> buffer;
                sscanf(buffer, "%d/%d/%d", &v[1], &vt[1], &vn[1]);
                memstream >> buffer;
                sscanf(buffer, "%d/%d/%d", &v[2], &vt[2], &vn[2]);

                v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
                v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

                vt[0] = (vt[0] < 0) ? vt[0] + numTexCoords - 1 : vt[0] - 1;
                vt[1] = (vt[1] < 0) ? vt[1] + numTexCoords - 1 : vt[1] - 1;
                vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

                vn[0] = (vn[0] < 0) ? vn[0] + numNormals - 1 : vn[0] - 1;
                vn[1] = (vn[1] < 0) ? vn[1] + numNormals - 1 : vn[1] - 1;
                vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

                addTrianglePosTexCoordNormal(numTriangles++, activeMaterial,
                    v[0], v[1], v[2], vt[0], vt[1], vt[2], vn[0], vn[1], vn[2]);

                v[1] = v[2];
                vt[1] = vt[2];
                vn[1] = vn[2];

                while (memstream.peek() != new_line)
                {
                	memstream >> buffer;
                	sscanf(buffer, "%d/%d/%d", &v[2], &vt[2], &vn[2]);

                    v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
                    vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;
                    vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

                    addTrianglePosTexCoordNormal(numTriangles++, activeMaterial,
                        v[0], v[1], v[2], vt[0], vt[1], vt[2], vn[0], vn[1], vn[2]);

                    v[1] = v[2];
                    vt[1] = vt[2];
                    vn[1] = vn[2];
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
            }
            else if (sscanf(buffer, "%d/%d", &v[0], &vt[0]) == 2) // v/vt
            {
            	memstream >> buffer;
                sscanf(buffer, "%d/%d", &v[1], &vt[1]);
                memstream >> buffer;
                sscanf(buffer, "%d/%d", &v[2], &vt[2]);

                v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
                v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

                vt[0] = (vt[0] < 0) ? vt[0] + numTexCoords - 1 : vt[0] - 1;
                vt[1] = (vt[1] < 0) ? vt[1] + numTexCoords - 1 : vt[1] - 1;
                vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

                addTrianglePosTexCoord(numTriangles++, activeMaterial,
                    v[0], v[1], v[2], vt[0], vt[1], vt[2]);

                v[1] = v[2];
                vt[1] = vt[2];

                while (memstream.peek() != new_line)
                {
                	memstream >> buffer;
                	sscanf(buffer, "%d/%d", &v[2], &vt[2]);

                    v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
                    vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

                    addTrianglePosTexCoord(numTriangles++, activeMaterial,
                        v[0], v[1], v[2], vt[0], vt[1], vt[2]);

                    v[1] = v[2];
                    vt[1] = vt[2];
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
            }
            else // v
            {
                sscanf(buffer, "%d", &v[0]);
                memstream >> buffer;
                sscanf(buffer, "%d", &v[1]);
                memstream >> buffer;
                sscanf(buffer, "%d", &v[2]);

                v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
                v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

                addTrianglePos(numTriangles++, activeMaterial, v[0], v[1], v[2]);

                v[1] = v[2];

                while (memstream.peek() != new_line)
                {
                	memstream >> buffer;
                	sscanf(buffer, "%d", &v[2]);

                    v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

                    addTrianglePos(numTriangles++, activeMaterial, v[0], v[1], v[2]);

                    v[1] = v[2];
                }

                if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
            }
            break;

        case 'u': // usemtl
        	memstream >> buffer;
            name = buffer;
            iter = m_materialCache.find(buffer);
            activeMaterial = (iter == m_materialCache.end()) ? 0 : iter->second;
            if(memstream.peek() == new_line)
			{
				//throw away new line char
				memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
            break;

        case 'v': // v, vn, or vt.
            switch (buffer[1])
            {
            case '\0': // v
            	memstream >> m_vertexCoords[3 * numVertices] >>
            			     m_vertexCoords[3 * numVertices + 1] >>
            			     m_vertexCoords[3 * numVertices + 2];
            	if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
                ++numVertices;
                break;

            case 'n': // vn
            	memstream >> m_normals[3 * numNormals] >>
            				 m_normals[3 * numNormals + 1] >>
            				 m_normals[3 * numNormals + 2];
            	if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
                ++numNormals;
                break;

            case 't': // vt
            	memstream >> m_textureCoords[2 * numTexCoords] >>
            				 m_textureCoords[2 * numTexCoords + 1];
            	if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
                ++numTexCoords;
                break;

            default:
                break;
            }
            break;

        default:
        	memstream.getline(buffer, 255);
        	if(memstream.peek() == new_line)
			{
				//throw away new line char
				memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
            break;
        }
    }

    memstream.clear();
}

bool Model::importMaterialsFromMemory(const char *matFileName)
{
	char* matlib = ResourceCache::GetInstance()->RequestSourceCode(string(matFileName));

	Material *pMaterial = 0;
	int illum = 0;
	int numMaterials = 0;
	char buffer[256] = {0};

	//line endings are controlled by perforce and are based on the local system,
	//so we can safely assume that under windows all input files will have windows EOL
	//and under Unix will have Unix EOL
#ifdef __WIN32__
	char new_line = '\r';
#else
	char new_line = '\n';
#endif

	string tmp(matlib);
	stringstream memstream(tmp, ios::in | ios::out);

	// Count the number of materials in the MTL file.
	while (memstream.good())
	{
		memstream >> buffer;

		switch (buffer[0])
		{
		case 'n': // newmtl
			++numMaterials;
			memstream.getline(buffer, 255);
			if(memstream.peek() == new_line)
			{
				//throw away new line char
				memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
			break;

		default:
			memstream.getline(buffer, 255);
			if(memstream.peek() == new_line)
			{
				//throw away new line char
				memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
			break;
		}
	}

	memstream.clear();
	memstream << matlib;
    memstream.seekg(0);
    
	m_numberOfMaterials = numMaterials;
	numMaterials = 0;
	m_materials.resize(m_numberOfMaterials);

	// Load the materials in the MTL file.
	while ( memstream.good() )
	{
		memstream >> buffer;

		switch (buffer[0])
		{
		case 'N': // Ns
			memstream >> pMaterial->mShininess;

			// Wavefront .MTL file shininess is from [0,1000].
			// Scale back to a generic [0,1] range.
			pMaterial->mShininess /= 10.0f;

			if(memstream.peek() == new_line)
			{
				//throw away new line char
				memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
			break;

		case 'K': // Ka, Kd, or Ks
			switch (buffer[1])
			{
			case 'a': // Ka
				memstream >> pMaterial->mAmbient[0] >>
						     pMaterial->mAmbient[1] >>
						     pMaterial->mAmbient[2];
				pMaterial->mAmbient[3] = 1.0f;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'd': // Kd
				memstream >> pMaterial->mDiffuse[0] >>
							 pMaterial->mDiffuse[1] >>
							 pMaterial->mDiffuse[2];
				pMaterial->mDiffuse[3] = 1.0f;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 's': // Ks
				memstream >> pMaterial->mSpecular[0] >>
						     pMaterial->mSpecular[1] >>
						     pMaterial->mSpecular[2];
				pMaterial->mSpecular[3] = 1.0f;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			default:
				memstream.getline(buffer, 255);
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;
			}
			break;

			case 'T': // Tr
				switch (buffer[1])
				{
				case 'r': // Tr
					memstream >> pMaterial->mAlpha;
					pMaterial->mAlpha = 1.0 - pMaterial->mAlpha;
					if(memstream.peek() == new_line)
					{
						//throw away new line char
						memstream.get(buffer[0]);
#ifdef __WIN32__
						//note that on win we just threw away '\r', but
						//we still need to throw away '\n'
						memstream.get(buffer[0]);
#endif
					}
					break;

				default:
					memstream.getline(buffer, 255);
					if(memstream.peek() == new_line)
					{
						//throw away new line char
						memstream.get(buffer[0]);
#ifdef __WIN32__
						//note that on win we just threw away '\r', but
						//we still need to throw away '\n'
						memstream.get(buffer[0]);
#endif
					}
					break;
				}
				break;

			case 'd':
				memstream >> pMaterial->mAlpha;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'i': // illum
				memstream >> illum;

				if (illum == 1)
				{
					pMaterial->mSpecular[0] = 0.0;
					pMaterial->mSpecular[1] = 0.0;
					pMaterial->mSpecular[2] = 0.0;
					pMaterial->mSpecular[3] = 1.0;
				}

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'm': // map_Kd, map_bump
				if (strstr(buffer, "map_Kd") != 0)
				{
					memstream >> buffer;
					ResourcePtr hres = ResourceCache::GetInstance()->RequestResource(buffer, RES_TEXTURE);
					ImageResource *himg = dynamic_cast<ImageResource*>(hres.get());
					pMaterial->mDiffuseTextures.push_back(himg->GetTexture());
				}
				else if (strstr(buffer, "map_bump") != 0)
				{
					memstream >> buffer;
					ResourcePtr hres = ResourceCache::GetInstance()->RequestResource(buffer, RES_TEXTURE);
					ImageResource *himg = dynamic_cast<ImageResource*>(hres.get());
					pMaterial->mBumpTextures.push_back(himg->GetTexture());
				} //TODO: add other texture types
				else
				{
					memstream.getline(buffer, 255);
				}

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'n': // newmtl

				memstream >> buffer;

				pMaterial = &m_materials[numMaterials];
				pMaterial->mAmbient[0] = 0.2;
				pMaterial->mAmbient[1] = 0.2;
				pMaterial->mAmbient[2] = 0.2;
				pMaterial->mAmbient[3] = 1.0;
				pMaterial->mDiffuse[0] = 0.8;
				pMaterial->mDiffuse[1] = 0.8;
				pMaterial->mDiffuse[2] = 0.8;
				pMaterial->mDiffuse[3] = 1.0;
				pMaterial->mSpecular[0] = 0.0;
				pMaterial->mSpecular[1] = 0.0;
				pMaterial->mSpecular[2] = 0.0;
				pMaterial->mSpecular[3] = 1.0;
				pMaterial->mShininess = 0.0;
				pMaterial->mAlpha = 1.0;
				pMaterial->mName = buffer;
				pMaterial->mDiffuseTextures.clear();
				pMaterial->mBumpTextures.clear();

				m_materialCache[pMaterial->mName] = numMaterials;
				++numMaterials;

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;
			
			case 'S': // shader pkg name
			
				memstream >> buffer;

				pMaterial->mShaderName = string(buffer);

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}

				break;

			default:
				memstream.getline(buffer, 255);
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;
		}
	}

	memstream.clear();
	delete matlib;

	return true;
}

}