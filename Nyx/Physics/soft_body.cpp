/*

A Soft Body

*/

#include "soft_body.h"
#include "Utils/log_manager.h"

using namespace std;

namespace NYX {

SoftBody::SoftBody(std::string name, uint id, btSoftBodyWorldInfo& softWorldInfo) :
	PhysicsBody(name, id),
	mBulletSoftBody(NULL),
	mBulletSoftWorldInfo(softWorldInfo),
	mKDF(0.5),
	mIterations(2)
{
	mBodyType = BDY_SOFT;
}

SoftBody::~SoftBody()
{
    mBulletSoftBody = NULL;
}

void SoftBody::SetMesh(float* rawVertices, int totalVertices, int stride, int* indices)
{
	mMeshInfo.vertices = rawVertices;
	mMeshInfo.indices = indices;
	mMeshInfo.totalVertices = totalVertices;
	mMeshInfo.stride = stride;
	mHasMesh = true;
}

void SoftBody::SetMaterialInfo(float kLST, float kAST, float kVST, float density)
{
	mMaterialInfo.kLST = kLST;
	mMaterialInfo.kAST = kAST;
	mMaterialInfo.kVST = kVST;
	mMaterialInfo.density = density;
}

void SoftBody::Finilize()
{
	if (!mHasMesh || mMeshInfo.indices == NULL)
	{
		LogManager::GetInstance()->LogMessage("Error! Cannot initialize Bullet soft body without a mesh!");
		return;
	}

	mBulletSoftBody = btSoftBodyHelpers::CreateFromTriMesh(mBulletSoftWorldInfo, mMeshInfo.vertices, mMeshInfo.indices, mMeshInfo.totalVertices/3);
	
	mBulletSoftBody->setTotalMass(mMass, true);

	mBulletSoftBody->transform(mBulletTransform);
	mBulletSoftBody->setVelocity( btVector3(mVelocity.X(), 
												  mVelocity.Y(),	
												  mVelocity.Z()) );
	
	//mBulletSoftBody->setFriction(mFriction);
	//mBulletSoftBody->setTotalDensity(mMaterialInfo.density);
	//mBulletSoftBody->setVolumeDensity(mMaterialInfo.density);

	btSoftBody::Material* mat = mBulletSoftBody->appendMaterial();

	mat->m_kLST = mMaterialInfo.kLST;
	mat->m_kAST = mMaterialInfo.kAST;
	mat->m_kVST = mMaterialInfo.kVST;
	mat->m_flags -=	btSoftBody::fMaterial::DebugDraw;

	mBulletSoftBody->generateBendingConstraints(2, mat);

	mBulletSoftBody->m_cfg.piterations	=	mIterations;
	mBulletSoftBody->m_cfg.kDF			=	mKDF;
	mBulletSoftBody->m_cfg.collisions	|=	btSoftBody::fCollision::VF_SS;

	mBulletSoftBody->randomizeConstraints();

	mIsInitialized = true;
}

void SoftBody::CalculateTotalForce()
{
	//mBulletRigidBody->applyForce(btVector3(0.21, 0.0, 0.0), btVector3(0.1, 0.0, 0.2));
}

float* SoftBody::GetUpdatedMesh()
{
	int triangles =  mBulletSoftBody->m_faces.size();
	int numVertex = triangles*3;
	int size = numVertex*6; //3 position + 3 normal
	float* mesh = new float[size];
	int triangle_stride = 6*3;

	btVector3 pos;
	btVector3 norm;

	for (int i = 0; i < triangles; i++)
	{
		//mBulletSoftBody->m_faces[i].m_normal; //per-triangle normal
		//vertex 1
		pos = mBulletSoftBody->m_faces[i].m_n[0]->m_x; //position
		norm = mBulletSoftBody->m_faces[i].m_n[0]->m_n;  //per-vertex normal
		
		mesh[i*triangle_stride + 0] = pos.x();
		mesh[i*triangle_stride + 1] = pos.y();
		mesh[i*triangle_stride + 2] = pos.z();
		mesh[i*triangle_stride + 3] = norm.x();
		mesh[i*triangle_stride + 4] = norm.y();
		mesh[i*triangle_stride + 5] = norm.z();

		//vertex 2
		pos = mBulletSoftBody->m_faces[i].m_n[1]->m_x; //position
		norm = mBulletSoftBody->m_faces[i].m_n[1]->m_n;

		mesh[i*triangle_stride + 6*1 + 0] = pos.x();
		mesh[i*triangle_stride + 6*1 + 1] = pos.y();
		mesh[i*triangle_stride + 6*1 + 2] = pos.z();
		mesh[i*triangle_stride + 6*1 + 3] = norm.x();
		mesh[i*triangle_stride + 6*1 + 4] = norm.y();
		mesh[i*triangle_stride + 6*1 + 5] = norm.z();

		//vertex 3
		pos = mBulletSoftBody->m_faces[i].m_n[2]->m_x; //position
		norm = mBulletSoftBody->m_faces[i].m_n[2]->m_n;

		mesh[i*triangle_stride + 6*2 + 0] = pos.x();
		mesh[i*triangle_stride + 6*2 + 1] = pos.y();
		mesh[i*triangle_stride + 6*2 + 2] = pos.z();
		mesh[i*triangle_stride + 6*2 + 3] = norm.x();
		mesh[i*triangle_stride + 6*2 + 4] = norm.y();
		mesh[i*triangle_stride + 6*2 + 5] = norm.z();
	}

	return mesh;
}

}


