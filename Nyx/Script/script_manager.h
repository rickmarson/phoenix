#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

/*

Script Manager (LUA)
 
*/

#include "LuaPlus.h"
#include "singleton.h"

#include <list>
#include <set>

namespace NYX {
/*
Script manager
*/
class ScriptManager : public SingletonClass<ScriptManager>
{
public:

    ScriptManager();
    ~ScriptManager();

    // initialize the scripting engine, callback, global functions and variables used across
    // scripts.
    bool Init();

	bool RunScript(std::string fileName);
	void RunCommand(std::string cmd);
	bool RegisterLuaFunction(std::string fileName, std::string funcName);
	bool RegisterLuaFunctions(std::string fileName, std::list<std::string> funcNames);
	LuaPlus::LuaObject GetLuaFunction(std::string funcName);
	void MakeTable(LuaPlus::LuaObject &obj);

    // Directly execute a pre-built function, assumes no inputs or outputs
    bool ExecuteFunction(std::string funcName);
    
	// Queue a function for later execution. Similarly to the event system, the main loop controls
    // when the queue is actually processed.
	// assumes no inputs or outputs
    bool QueueFunctionExecution(std::string funcName);

    // Batch execute queued functions.
    void ProcessQueuedFunctions();

private:

	// List of functions awaiting execution. Newly queued functions are stored in a back (inactive) queue
    // for added safety. Once the active queue is empty, the two are swapped.
    typedef std::list<LuaPlus::LuaObject> ExecQueue;

    // utility function used by the script engine to communicate issues to the host application.
    // registered as a global function.
    int ScriptMessageCallback(LuaPlus::LuaState *state);

	void Print(LuaPlus::LuaObject message);

    // swap the current active and inactive process queues;
    inline void SwapQueues();
    // determines which is the inactive queue (where newly queued functions are stored)
    inline unsigned short WhichQueue();

	LuaPlus::LuaStateOwner mLuaState;

    ExecQueue mPendingFunctions[2];

    unsigned short mActiveQueue; // the index of the queue that will be processed in the next loop
    float mMaxProcessingTime; // maximum time allowed for queue processing in each loop

	std::set<std::string> mRegisteredLuaFunctions;
};


}

#endif // SCRIPTMANAGER_H
