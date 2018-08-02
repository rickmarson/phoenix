/*

Particle Effect

*/

#ifndef PARTICLEFX_H
#define PARTICLEFX_H

#include "fx_manager.h"
#include "irenderer.h"
#include "material.h"
#include "Script/script_manager.h"

namespace NYX {

//Required Classes
    
class VertexBuffer;
class IndexBuffer;
class VertexArrayObject;
class Effect;

struct NYX_EXPORT Particle
{
	Particle() {
		//fixes VS complaing about uninitialized values
		memset(position, 0, 3*sizeof(float));
		memset(velocity, 0, 3*sizeof(float));
		life = 0;
	}

	float position[3];
	float velocity[3];
	float life;
};

class NYX_EXPORT ParticleFX 
{

public:
	
	ParticleFX(IRenderer* renderer) :
		mRenderer(renderer),
		mParticleCount(0),
		bScriptCloudGeneration(false)
	{ }
    
    ~ParticleFX();

	uint ParticleCount(); 
	MaterialPtr GetMaterial();
    VertexBuffer* GetVertexBuffer()                 { return mVertexBuffer; }
    VertexArrayObject* GetVAO();

	void Initialize(std::string programName, std::string kernelName, uint size, std::string materialName, std::string &shaderName, Effect **prog);
	void ExecuteKernel(float timeStep);
    void UseScriptParticleGeneration(std::string source, std::string funcName);

protected:

	IRenderer* mRenderer;
	MaterialPtr mMaterial;

	uint mParticleCount;

    VertexArrayObject* mVertexArray = nullptr;
    VertexBuffer* mVertexBuffer = nullptr;
    
	std::string mCLProgramName;
	std::string mCLKernelName;
	cl_program mCLProgram;
    cl_kernel mCLKernel;
	cl_mem mBuffer0; //initial state to reset to. 

	//script integration
	bool bScriptCloudGeneration;
    std::string mScriptFunctionName;

	virtual void GenerateParticleCloud(cl_command_queue &queue);
};

inline MaterialPtr ParticleFX::GetMaterial() { return mMaterial; }
inline uint ParticleFX::ParticleCount() { return mParticleCount; }
inline VertexArrayObject* ParticleFX::GetVAO() { return mVertexArray; }

}

#endif //PARTICLEFX_H
