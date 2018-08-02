/*

Particle Effect

*/

#include "particlefx.h"
#include "Cache/resource_cache.h"
#include "Renderer/vertex_buffer.h"
#include "Renderer/index_buffer.h"
#include "Renderer/vertex_array_object.h"

#include "Renderer/ogl/gl_vertex_buffer.h"

using namespace LuaPlus;

namespace NYX {

ParticleFX::~ParticleFX()
{
    clReleaseMemObject(mBuffer0);
    delete mVertexBuffer;
    delete mVertexArray;
}
    
void ParticleFX::Initialize(string programName, string kernelName, uint size, string materialName, string &shaderName, Effect** prog)
{
	mCLProgramName = programName;
	mCLKernelName = kernelName;
	mParticleCount = size;

	//load material
	ResourcePtr res = ResourceCache::GetInstance()->RequestResource(materialName, RES_MATERIAL);
	mMaterial = dynamic_cast<MaterialResource*>(res.get())->GetMaterial();

	mCLProgram = FXManager::GetInstance()->CreateProgram(programName);
	mCLKernel = FXManager::GetInstance()->GetKernel(programName, kernelName);

	shaderName = mMaterial->GetShaderName();
	*prog = mRenderer->CreateShaderProgram(shaderName);
	mMaterial->SetShaderProg(*prog);

    mVertexBuffer = mRenderer->CreateVertexBuffer();
    mVertexBuffer->Create( sizeof(Particle), mParticleCount );
    mVertexArray = mRenderer->CreateVertexArrayObject();
    mVertexArray->Create();
    
    mVertexBuffer->Bind();
    mVertexArray->Bind();
    
    mVertexArray->EnableVertexAttribute(*prog, "vVertex", 3, sizeof(float)*7, (sizeof(float)*0));
    mVertexArray->EnableVertexAttribute(*prog, "vVelocity", 4, sizeof(float)*7, (sizeof(float)*3));
   
    mVertexArray->Unbind();
    mVertexBuffer->Unbind();

	cl_context ctx = FXManager::GetInstance()->GetCLContext();
	cl_command_queue queue = FXManager::GetInstance()->GetCommandQueue();
	cl_int err;

    err = mVertexBuffer->CreateCLBufferFromThis(ctx);
    
	if (err != 0)
		LogManager::GetInstance()->LogMessage("Error! Failed to create CL buffer from GL VBO.");

	mBuffer0 = clCreateBuffer(ctx, CL_MEM_READ_ONLY, sizeof(Particle)*mParticleCount, NULL, &err); //we still need to load the buffer!

	if (err != 0)
		LogManager::GetInstance()->LogMessage("Error! Failed to create CL buffer.");

	GenerateParticleCloud(queue);

	err = clFinish(queue);
}

void ParticleFX::UseScriptParticleGeneration(string source, string funcName)
{
	ScriptManager* scriptManager = ScriptManager::GetInstance();

	scriptManager->RegisterLuaFunction(source, funcName);
    
    mScriptFunctionName = funcName;
	bScriptCloudGeneration = true;
}

void ParticleFX::GenerateParticleCloud(cl_command_queue &queue)
{
	Particle* particles = new Particle[mParticleCount];
	
	if (bScriptCloudGeneration)
	{
		ScriptManager *pScript = ScriptManager::GetInstance();
		
        LuaObject func_object = pScript->GetLuaFunction(mScriptFunctionName);
		LuaFunction<LuaObject> GenParticles( func_object );

		LuaObject cloud = GenParticles(mParticleCount);
		
		if (!cloud.IsTable())
			return;
		
		for (uint i = 0; i < mParticleCount; i++)
		{
			particles[i].position[0] = cloud[i*7].GetFloat();
			particles[i].position[1] = cloud[i*7 + 1].GetFloat();
			particles[i].position[2] = cloud[i*7 + 2].GetFloat();

			particles[i].velocity[0] = cloud[i*7 + 3].GetFloat();
			particles[i].velocity[1] = cloud[i*7 + 4].GetFloat();
			particles[i].velocity[2] = cloud[i*7 + 5].GetFloat();

			particles[i].life = cloud[i*7 + 6].GetFloat(); // s
		}

	}

	//populate buffers
    cl_int error = mVertexBuffer->LockCL(queue);
    error |= mVertexBuffer->UpdateCLBuffer(queue, sizeof(Particle)*mParticleCount, particles);
    error |= mVertexBuffer->UnlockCL(queue);
    
	error |= clEnqueueWriteBuffer(queue, mBuffer0, CL_TRUE, 0, sizeof(Particle)*mParticleCount, particles, 0, NULL, NULL);

	if (error != 0)
		LogManager::GetInstance()->LogMessage("Error! Failed to populate FX buffers!");

	delete [] particles;
}

void ParticleFX::ExecuteKernel(float timeStep)
{
	//cl_context ctx = FXManager::GetInstance()->GetCLContext();
	cl_command_queue queue = FXManager::GetInstance()->GetCommandQueue();
	cl_int error;

    error = mVertexBuffer->LockCL(queue);

	error = clSetKernelArg(mCLKernel, 0, sizeof(cl_mem), mVertexBuffer->GetSharedCLBuffer());
	error |= clSetKernelArg(mCLKernel, 1, sizeof(cl_mem), &mBuffer0);
	error |= clSetKernelArg(mCLKernel, 2, sizeof(float), &timeStep);
	error |= clSetKernelArg(mCLKernel, 3, sizeof(int), &mParticleCount);

	const size_t local_ws = 100;
	const size_t total_ws = mParticleCount; 

	error = clEnqueueNDRangeKernel(queue, mCLKernel, 1, NULL, &total_ws, &local_ws, 0, NULL, NULL);

    error = mVertexBuffer->UnlockCL(queue);

	error = clFinish(queue);
}

}

