/*
 
 Particle Effects Manager
 
 */

#include "fx_manager.h"
#if PLATFORM_MAC
#include <OpenGL/OpenGL.h>
#else
#include "SDL/SDL_platform.h"
#endif
#include "Cache/resource_cache.h"
#include "irenderer.h"

#define MAX_CL_DEVICES 5

namespace NYX {


FXManager::FXManager() :
	mLastError(0)
{
	
}

FXManager::~FXManager()
{
	for (map<string, cl_kernel>::iterator it = mKernels.begin();
		it != mKernels.end(); ++it)
	{
		clReleaseKernel((*it).second);
	}

	clReleaseCommandQueue(mCommandQueue);
	clReleaseContext(mCLContext);
}

void FXManager::Initialize(SDL_Window *win, SDL_SysWMinfo winInfo, IRenderer* renderer)
{
    mRenderer = renderer;
    
	//init CL context from GL context
	cl_device_id devices[MAX_CL_DEVICES];
	cl_uint dev_count = 0; //the actual number of available devices.

	mLastError = clGetPlatformIDs(1, &mCLPlatform, NULL);
	checkError(mLastError, "Error getting platform IDs");

	dumpPlatformInfo(mCLPlatform);

	mLastError = clGetDeviceIDs(mCLPlatform, CL_DEVICE_TYPE_ALL, MAX_CL_DEVICES, devices, &dev_count);
	checkError(mLastError, "Could not find a device.");

	//one more safety check
	if (dev_count == 0)
	{
		LogManager::GetInstance()->LogMessage("Error! No devices found. Terminating Program.");
		exit(-1);
	}

	//search for a GPU
	bool gpu_found = false;
	cl_device_id cl_cpu = nullptr;

	for (cl_uint dev = 0; dev < dev_count; dev++)
	{
		cl_device_type type;
		size_t size;

		clGetDeviceInfo(devices[dev], CL_DEVICE_TYPE, sizeof(type), &type, &size);
		if (type == CL_DEVICE_TYPE_GPU)
		{
			mCLDevice = devices[dev];
			gpu_found = true;
			break;
		}
		else if (type == CL_DEVICE_TYPE_CPU)
			cl_cpu = devices[dev];
	}

	//fall back to CPU if no suitable GPU was found
	if (!gpu_found)
		mCLDevice = cl_cpu;

	dumpDeviceInfo(mCLDevice);

    mCLContext = mRenderer->InitialiseCLContextFromGLContext( &mCLDevice, mCLPlatform, cl_cpu, mLastError );

	mCommandQueue = clCreateCommandQueue(mCLContext, mCLDevice, 0, &mLastError);
	checkError(mLastError, "Error generating command queue.");
}

void FXManager::checkError(cl_int error_code, string message)
{
	if (error_code != CL_SUCCESS)
	{
		string msg = message + "\n";
		msg += "Code: ";
		msg += error_code;
		LogManager::GetInstance()->LogMessage(msg);
		exit(error_code);
	}
}

void FXManager::displayBuildLog(cl_program program, cl_device_id device)
{
	// Shows the log
	char* build_log;
	size_t log_size;
	// First call to know the proper size
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	build_log = new char[log_size+1];
	// Second call to get the log
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
	build_log[log_size] = '\0';
	LogManager::GetInstance()->LogMessage(build_log);
	delete[] build_log;
}

void FXManager::dumpPlatformInfo(cl_platform_id platform)
{
	char buffer[255];
	size_t size;
	string msg;

	clGetPlatformInfo(platform, CL_PLATFORM_NAME, 255, buffer, &size);
	msg = "Platform Name: ";
	msg += buffer;
	LogManager::GetInstance()->LogMessage(msg);
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 255, buffer, &size);
	msg = "Platform Vendor: ";
	msg += buffer;
	LogManager::GetInstance()->LogMessage(msg);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 255, buffer, &size);
	msg = "OpenCL Version: ";
	msg += buffer;
	LogManager::GetInstance()->LogMessage(msg);
	clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, 255, buffer, &size);
	msg = "OpenCL Profile: ";
	msg += buffer;
	LogManager::GetInstance()->LogMessage(msg);

#ifdef __DEBUG__

	clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, 255, buffer, &size);
	string tmpMsg(buffer);
	msg = "OpenCL Supported Extensions: \n\t\t\t";
	size_t start = 0;
	size_t stop = tmpMsg.find(" ", start);
	while (stop != string::npos)
	{
		tmpMsg.replace(stop, 1, "\n\t\t\t");
		start = stop + 1;
		stop = tmpMsg.find(" ", start);
	}
	msg += tmpMsg;
	LogManager::GetInstance()->LogMessage(msg);

#endif
}

void FXManager::dumpDeviceInfo(cl_device_id device)
{
	char buffer[255];
	cl_uint buffer_i;
	cl_uint wi_dim;
	size_t buffer_sz;
	cl_device_type type;
	size_t size;
	string msg;
	
	clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, &size);
	if (type == CL_DEVICE_TYPE_GPU)
		LogManager::GetInstance()->LogMessage("Device Type: CL_DEVICE_TYPE_GPU");
	else if (type == CL_DEVICE_TYPE_CPU)
		LogManager::GetInstance()->LogMessage("Device Type: CL_DEVICE_TYPE_CPU");
	else if (type == CL_DEVICE_TYPE_ACCELERATOR)
		LogManager::GetInstance()->LogMessage("Device Type: CL_DEVICE_TYPE_ACCELERATOR");
	else if (type == CL_DEVICE_TYPE_DEFAULT)
		LogManager::GetInstance()->LogMessage("Device Type: CL_DEVICE_TYPE_DEFAULT");
	else
	{
		msg = "Unrecognised Device Type: ";
        msg += std::to_string(type);
		LogManager::GetInstance()->LogMessage(msg);
	}

	clGetDeviceInfo(device, CL_DEVICE_NAME, 255, buffer, &size);
	msg = "Device Name: ";
	msg += buffer;
	LogManager::GetInstance()->LogMessage(msg);
	clGetDeviceInfo(device, CL_DEVICE_VENDOR, 255, buffer, &size);
	msg = "Device Vendor: ";
	msg += buffer;
	LogManager::GetInstance()->LogMessage(msg);
	clGetDeviceInfo(device, CL_DEVICE_VENDOR_ID, sizeof(cl_uint), &buffer_i, &size);
	msg = "Device Vendor ID: ";
    msg += std::to_string(buffer_i);
	LogManager::GetInstance()->LogMessage(msg);
	clGetDeviceInfo(device, CL_DEVICE_VERSION, 255, buffer, &size);
	msg = "Device Version: ";
	msg += buffer;
	LogManager::GetInstance()->LogMessage(msg);
	clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &buffer_i, &size);
	msg = "Max Compute Units: ";
    msg += std::to_string(buffer_i);
	LogManager::GetInstance()->LogMessage(msg);
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &wi_dim, &size);
	msg = "Max Work Item Dimensions: ";
    msg += std::to_string(wi_dim);
	LogManager::GetInstance()->LogMessage(msg);

	size_t *max_sizes = new size_t[wi_dim];
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*wi_dim, max_sizes, &size);
	msg = "Max Work Item Sizes: ";
	for (cl_uint i = 0; i < wi_dim; i++)
	{
        msg += std::to_string(max_sizes[i]);
		msg += " ";
	}
	LogManager::GetInstance()->LogMessage(msg);

	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &buffer_sz, &size);
	msg = "Max Work Group Size: ";
    msg += std::to_string(buffer_sz);
	LogManager::GetInstance()->LogMessage(msg);

	delete max_sizes;
}

cl_program FXManager::CreateProgram(std::string programName)
{
	if (mCompiledPrograms.find(programName) != mCompiledPrograms.end())
		return mCompiledPrograms[programName];

	char *src = ResourceCache::GetInstance()->RequestSourceCode(programName);
	size_t src_size = strlen(src);

	cl_program program = clCreateProgramWithSource(mCLContext, 1, (const char**)&src, &src_size, &mLastError);
	checkError(mLastError, "Failed to create program.");

	mLastError = clBuildProgram(program, 1, &mCLDevice, NULL, NULL, NULL);
	// Shows the log
	displayBuildLog(program, mCLDevice);
	checkError(mLastError, "Failed to compile program.");

	mCompiledPrograms[programName] = program;

	delete src;

	return program;
}

cl_kernel FXManager::GetKernel(std::string programName, std::string kernelName)
{
	if (mKernels.find(kernelName) == mKernels.end())
	{
		//create kernel
		bool kernelExtracted = false;
		cl_kernel kernel;
		cl_program program;

		if (mCompiledPrograms.find(programName) == mCompiledPrograms.end())
		{
			program = CreateProgram(programName);
		}
		else
		{
			program = mCompiledPrograms[programName];
		}
		
		kernel = clCreateKernel(program, kernelName.c_str(), &mLastError);
		if (mLastError == 0)
		{
			mKernels[kernelName] = kernel;
			kernelExtracted = true;
		}
		
		if (kernelExtracted)
			return kernel;
		else
		{
			checkError(mLastError, "Failed to create kernel.");
			return NULL;
		}
	}
	else
	{
		return (*mKernels.find(kernelName)).second;
	}
}

}