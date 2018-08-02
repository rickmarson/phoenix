/*

Script Manager (LUA)

*/

#include "script_manager.h"
#include "log_manager.h"
#include "resource_cache.h"

#include <assert.h>
#include <sstream>
#include <ctime>

using namespace std;

namespace nyx {

ScriptManager::ScriptManager()  :
	mLuaState(true) //init standard library
{
    mActiveQueue = 0;
    mMaxProcessingTime = 0.01; //10 milli-seconds.

}

ScriptManager::~ScriptManager()
{
    LogManager::GetInstance()->LogMessage( "Scripting Engine Shutting Down." );
}

bool ScriptManager::Init()
{
    LogManager::GetInstance()->LogMessage("Initializing Scripting Engine...");

	//class emulation
	LuaObject metaTableObj = mLuaState->GetGlobals().CreateTable("ScriptManagerMetaTable");
    metaTableObj.SetObject("__index", metaTableObj);

	metaTableObj.RegisterObjectDirect("PrintMessage", (ScriptManager*)0, &ScriptManager::Print);

    LuaObject interopObjLua = mLuaState->BoxPointer(this);
    interopObjLua.SetMetaTable(metaTableObj);
    mLuaState->GetGlobals().SetObject("ScriptManager", interopObjLua);
	
	//set callback
	LuaObject globals = mLuaState->GetGlobals();
	globals.Register("LOG", *this, &ScriptManager::ScriptMessageCallback);

    return true;

}

bool ScriptManager::RegisterLuaFunction(std::string fileName, std::string funcName) 
{
	char* src = ResourceCache::GetInstance()->RequestSourceCode(fileName);

	if (src == NULL)
		return false;

	mLuaState->DoString(src);
	mRegisteredLuaFunctions.insert(funcName);

	delete src;
	return true;
}

bool ScriptManager::RegisterLuaFunctions(std::string fileName, std::list<std::string> funcNames)
{
	char* src = ResourceCache::GetInstance()->RequestSourceCode(fileName);

	if (src == NULL)
		return false;

	mLuaState->DoString(src);

	for (std::list<std::string>::iterator it = funcNames.begin(); it != funcNames.end(); ++it)
		mRegisteredLuaFunctions.insert(*it);

	delete src;
	return true;
}

LuaObject ScriptManager::GetLuaFunction(std::string funcName)
{
	set<string>::iterator it = mRegisteredLuaFunctions.find(funcName);

	if ( it == mRegisteredLuaFunctions.end())
		LogManager::GetInstance()->LogMessage( string("Error: Could not find Lua function: ") + funcName );

	return mLuaState->GetGlobal(funcName.c_str());
}

void ScriptManager::MakeTable(LuaObject &obj)
{
	obj.AssignNewTable(mLuaState);
}

bool ScriptManager::RunScript(std::string fileName) 
{
	char* src = ResourceCache::GetInstance()->RequestSourceCode(fileName);

	if (src == NULL)
		return false;

	mLuaState->DoString(src);

	delete src;
	return true;
}

void ScriptManager::RunCommand(std::string cmd)
{
	mLuaState->DoString(cmd.c_str());
}


bool ScriptManager::ExecuteFunction(string funcName)
{
	LuaObject luaFunctionProxy;
	
	set<string>::iterator it = mRegisteredLuaFunctions.find(funcName);

    if ( it == mRegisteredLuaFunctions.end())
        return false;

    luaFunctionProxy = mLuaState->GetGlobal(funcName.c_str());

	static_cast< LuaFunction<float> >(luaFunctionProxy)();

	return true;
}

bool ScriptManager::QueueFunctionExecution(std::string funcName)
{
	LuaObject luaFunctionProxy = GetLuaFunction(funcName);

	// store in the inactive queue
	unsigned short backQueue = WhichQueue();

	mPendingFunctions[backQueue].push_back(luaFunctionProxy);

	return true;

}

void ScriptManager::ProcessQueuedFunctions()
{
	ExecQueue & activeQueue = mPendingFunctions[mActiveQueue];

	// exit now if there are no pending functions
	if (activeQueue.empty())
	{
		SwapQueues();
		return;
	}

	clock_t startTime, time;
	float dt = 0.0; //elapsed time in seconds.

	startTime = clock();

	// TODO: since the execution time for each function is unknown and realistically
	// will vary wildly between functions, it might be necessary to pause a function call
	// if the time-out has passed and resume it the next time around.
	while (dt <= mMaxProcessingTime)
	{
		ExecQueue::iterator qf;
		qf = activeQueue.begin();

		if (qf == activeQueue.end())
			break;

		LuaObject func = *qf;

		activeQueue.pop_front();

		static_cast< LuaFunction<float> >(func)();
	
		time = clock();
		dt = (time - startTime)/(float)CLOCKS_PER_SEC;
	}

	if (activeQueue.empty())
		SwapQueues();
}

int ScriptManager::ScriptMessageCallback(LuaState* state)
{
	LuaStack args(state);
	string msg = "Lua Message: ";
	msg += args[1].GetString();

	LogManager::GetInstance()->LogMessage( msg );

	return 0;
}

void ScriptManager::Print(LuaObject message)
{
	if (message.IsString())
    {
        const char *msg = message.ToString();

		LogManager::GetInstance()->LogMessage( msg );
    }
}

unsigned short ScriptManager::WhichQueue()
{
	// store in the inactive queue
	unsigned short backQueue;

	if (mActiveQueue == 0)
		backQueue = 1;
	else
		backQueue = 0;

	return backQueue;
}

void ScriptManager::SwapQueues()
{
	if (mActiveQueue == 0)
		mActiveQueue = 1;
	else
		mActiveQueue = 0;
}

}
