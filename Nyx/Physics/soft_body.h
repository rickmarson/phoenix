#ifndef SOFTBODY_H
#define SOFTBODY_H

/*
 
 A Soft Body
 
 */

#include "body.h"
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

namespace NYX {

/*
Wrapper for Bullet Rigid bodies
*/
class SoftBody : public PhysicsBody
{
	//temporary storage of original mesh data before bullet soft body initialization
	struct MeshInfo {

		MeshInfo() :
			vertices(NULL),
			indices(NULL),
			totalVertices(0),
			stride(0)
		{}
		~MeshInfo() {
			vertices = NULL;
			indices = NULL;
		}
		void Reset() {
			vertices = NULL;
			indices = NULL;
			totalVertices = 0;
			stride = 0;
		}

		float* vertices; //groups of x,y,z coordinates
		int* indices;
		int totalVertices;
		int stride;

	}mMeshInfo;

	struct MaterialInfo {
		MaterialInfo() :
			kLST(0.5),
			kAST(0.0),
			kVST(0.0),
			density(0.0)
		{}

		float kLST;
		float kAST; 
		float kVST;
		float density;

	}mMaterialInfo;

public:

    SoftBody(std::string name, uint id, btSoftBodyWorldInfo& softWorldInfo);
    ~SoftBody();

	void SetMesh(float* rawVertices, int totalVertices, int stride, int* indices = NULL);
	void SetMaterialInfo(float kLST, float kAST, float kVST, float density);
	void SetConfigurationOptions(int iterations, float kdf);

	void Finilize();

	btSoftBody* GetBulletSoftBody();

	void CalculateTotalForce();
	float* GetUpdatedMesh(); //vertex position and normals only

private:

	btSoftBody* mBulletSoftBody;
	btSoftBodyWorldInfo& mBulletSoftWorldInfo;

	int mIterations;
	float mKDF; //dynamic friction coefficient
};

inline btSoftBody* SoftBody::GetBulletSoftBody() { return mBulletSoftBody; }
}

#endif // SOFTBODY_H
