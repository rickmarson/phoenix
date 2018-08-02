/*

Particle Effects Manager

*/

#ifndef FXMANAGER_H
#define FXMANAGER_H

#if PLATFORM_MAC
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <OpenCL/OpenCL.h>
#else
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#include <CL/opencl.h>
#endif

#include "log_manager.h"

#include <map>

using namespace std;

namespace NYX {

// Required Classes
class IRenderer;
    
class NYX_EXPORT FXManager : public SingletonClass<FXManager>
{
public:

	FXManager();
    ~FXManager();

	void Initialize(SDL_Window *win, SDL_SysWMinfo winInfo, IRenderer* renderer);
	cl_context GetCLContext();
	cl_command_queue GetCommandQueue();
	cl_program CreateProgram(std::string programName);
	cl_kernel GetKernel(std::string programName, std::string kernelName);

private:

    IRenderer* mRenderer;
	cl_int mLastError;
	cl_platform_id mCLPlatform = nullptr;
	cl_context mCLContext = nullptr;
	cl_command_queue mCommandQueue = nullptr;
	cl_device_id mCLDevice = nullptr;

	std::map<std::string, cl_program> mCompiledPrograms;
	std::map<std::string, cl_kernel> mKernels;

	void checkError(cl_int error_code, std::string message);
	void displayBuildLog(cl_program program, cl_device_id device);
	void dumpPlatformInfo(cl_platform_id platform);
	void dumpDeviceInfo(cl_device_id device);
};

inline cl_context FXManager::GetCLContext() { return mCLContext; }
inline cl_command_queue FXManager::GetCommandQueue() { return mCommandQueue; }

}

#endif // FXMANAGER_H
