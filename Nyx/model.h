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

#if !defined(MODEL_H)
#define MODEL_H

#include <cstdio>
#include <map>
#include <vector>

#include "mesh.h"

//-----------------------------------------------------------------------------
// Alias|Wavefront OBJ file loader.
//
// This OBJ file loader contains the following restrictions:
// 1. Group information is ignored. Faces are grouped based on the material
//    that each face uses.
// 2. Object information is ignored. This loader will merge everything into a
//    single object.
// 3. The MTL file must be located in the same directory as the OBJ file. If
//    it isn't then the MTL file will fail to load and a default material is
//    used instead.
// 4. This loader triangulates all polygonal faces during importing.
//-----------------------------------------------------------------------------

namespace NYX {
    
class NYX_EXPORT Model
{
public:

	//Note: the original Material, Vertex and Mesh structs have been substituted with their respective NYX classes 
	//in order to allow for a tighter integration of the Model class. 

    Model();
    ~Model();

    void destroy();

	//Note: the original import functions (which imported a model only from file) have been removed 
	//since files are never accessed directly here. This allows for easier storage of models in 
	//compressed archives. 

    //**** added by R.Marson
    //allows loading objects from a memory stream, works with PhysFS and can read models directly from
    //archives.
    bool importFromMemory(const char *stream, bool rebuildNormals = false);
    //****

    void normalize(float scaleTo = 1.0f, bool center = true);
    void reverseWinding();
 
    // Getter methods.

    void getCenter(float &x, float &y, float &z) const;
    float getWidth() const;
    float getHeight() const;
    float getLength() const;
    float getRadius() const;

    const int *getIndexBuffer() const;
    int getIndexSize() const;

    const Material &getMaterial(int i) const;
	const MeshPtr getMesh(int i) const;

    int getNumberOfIndices() const;
    int getNumberOfMaterials() const;
    int getNumberOfMeshes() const;
    int getNumberOfTriangles() const;
    int getNumberOfVertices() const;

    const std::string &getPath() const;

    const Vertex &getVertex(int i) const;
    const Vertex *getVertexBuffer() const;
    int getVertexSize() const;

    bool hasNormals() const;
    bool hasPositions() const;
    bool hasTangents() const;
    bool hasTextureCoords() const;

    void scale(float scaleFactor, float offset[3]);
    
private:
    void addTrianglePos(int index, int material,
        int v0, int v1, int v2);
    void addTrianglePosNormal(int index, int material,
        int v0, int v1, int v2,
        int vn0, int vn1, int vn2);
    void addTrianglePosTexCoord(int index, int material,
        int v0, int v1, int v2,
        int vt0, int vt1, int vt2);
    void addTrianglePosTexCoordNormal(int index, int material,
        int v0, int v1, int v2,
        int vt0, int vt1, int vt2,
        int vn0, int vn1, int vn2);
    int addVertex(int hash, const Vertex *pVertex);
    void bounds(float center[3], float &width, float &height,
        float &length, float &radius) const;
    void buildMeshes();
    void generateNormals();
    void generateTangents();

    //**** overloads added by R.Marson
	//Note: again, the original methods that worked on files have been removed. 
    //allows loading objects from a memory stream, works with PhysFS and can read models directly from
    //archives.
    void importGeometryFirstPass(const char *stream);
    void importGeometrySecondPass(const char *stream);
    //----------------------------------------------------------------------

    //**** added by R.Marson
    //allows loading objects from a memory stream, works with PhysFS and can read models directly from
    //archives.
    bool importMaterialsFromMemory(const char *matFileName);
    //****

    bool m_hasPositions;
    bool m_hasTextureCoords;
    bool m_hasNormals;
    bool m_hasTangents;

    int m_numberOfVertexCoords;
    int m_numberOfTextureCoords;
    int m_numberOfNormals;
    int m_numberOfTriangles;
    int m_numberOfMaterials;
    int m_numberOfMeshes;

    float m_center[3];
    float m_width;
    float m_height;
    float m_length;
    float m_radius;

    std::string m_directoryPath;
    std::vector<MeshPtr> m_meshes;
    std::vector<Material> m_materials;
    std::vector<Vertex> m_vertexBuffer;
    std::vector<int> m_indexBuffer;
    std::vector<int> m_attributeBuffer;
    std::vector<float> m_vertexCoords;
    std::vector<float> m_textureCoords;
    std::vector<float> m_normals;

    std::map<std::string, int> m_materialCache;
    std::map<int, std::vector<int> > m_vertexCache;
};

//-----------------------------------------------------------------------------

inline void Model::getCenter(float &x, float &y, float &z) const
{ x = m_center[0]; y = m_center[1]; z = m_center[2]; }

inline float Model::getWidth() const
{ return m_width; }

inline float Model::getHeight() const
{ return m_height; }

inline float Model::getLength() const
{ return m_length; }

inline float Model::getRadius() const
{ return m_radius; }

inline const int *Model::getIndexBuffer() const
{ return &m_indexBuffer[0]; }

inline int Model::getIndexSize() const
{ return static_cast<int>(sizeof(int)); }

inline const Material &Model::getMaterial(int i) const
{ return m_materials[i]; }

inline const MeshPtr Model::getMesh(int i) const
{ return m_meshes[i]; }

inline int Model::getNumberOfIndices() const
{ return m_numberOfTriangles * 3; }

inline int Model::getNumberOfMaterials() const
{ return m_numberOfMaterials; }

inline int Model::getNumberOfMeshes() const
{ return m_numberOfMeshes; }

inline int Model::getNumberOfTriangles() const
{ return m_numberOfTriangles; }

inline int Model::getNumberOfVertices() const
{ return static_cast<int>(m_vertexBuffer.size()); }

inline const std::string &Model::getPath() const
{ return m_directoryPath; }

inline const Vertex &Model::getVertex(int i) const
{ return m_vertexBuffer[i]; }

inline const Vertex *Model::getVertexBuffer() const
{ return &m_vertexBuffer[0]; }

inline int Model::getVertexSize() const
{ return static_cast<int>(sizeof(Vertex)); }

inline bool Model::hasNormals() const
{ return m_hasNormals; }

inline bool Model::hasPositions() const
{ return m_hasPositions; }

inline bool Model::hasTangents() const
{ return m_hasTangents; }

inline bool Model::hasTextureCoords() const
{ return m_hasTextureCoords; }

}

#endif
