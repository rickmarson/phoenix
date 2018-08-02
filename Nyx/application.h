/*

Application Class

*/

#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once

#include "singleton.h"
#include "window.h"
#include "Utils/file_manager.h"
#include "Cache/resource_cache.h"
#include "Events/event_manager.h"
#include "Script/script_manager.h"

namespace NYX {

// Required Classes
typedef std::shared_ptr< NYX::FileManager > FileManagerPtr;
typedef std::shared_ptr< NYX::LogManager > LoggerPtr;
typedef std::shared_ptr< NYX::EventManager > EventManagerPtr;
typedef std::shared_ptr< NYX::ScriptManager > ScriptManagerPtr;
typedef std::shared_ptr< NYX::Window > WindowPtr;
typedef std::shared_ptr< NYX::ResourceCache > CachePtr;

class Application;
typedef shared_ptr< NYX::Application > ApplicationPtr;
    
class Application : public SingletonClass<Application>
{
public:

	Application(char* argv0, std::string logFileName = "log.txt");
	virtual ~Application();

	void RegisterCacheSearchPath(std::string mountPoint, std::string path);
    void Initialise( std::string config_file );
	void ShowWindow(bool show);
	void SetAnimationRefreshRate(uint rate); //in ms
    void Run( void );

	WindowPtr GetWindowPtr() { return pWindow; }
	uint	  GetAnimationRate() { return mAnimationRefreshRate; }

public:

	uint mLoopStartTime;
	uint mCurTime;
	bool bIsRunning;

	int mMouseSensitivity;

protected:
    
    void CreateGameWindow(std::string name, int w, int h, int depth, NYX::eRenderer eBackend);
    void LoadConfig( std::string file_name );
                     
protected:

    struct WindowProperties
    {
        std::string title;
        int width;
        int height;
        int depth;
        NYX::eRenderer backend;
    };
    
    FileManagerPtr pFileManager;
	LoggerPtr pLogger;
	EventManagerPtr pEventMng;
	ScriptManagerPtr pScriptManager;
	WindowPtr pWindow;
	CachePtr pResourceCache;

	std::string mLogFile;
	char* pArgv0; //keep a record of the full program name with path. 
	
	uint mAnimationRefreshRate; //ms, default 20 ms or 50 Hz
    
    WindowProperties mWindowProps;
    
    // ui event handling
    bool mMouseButtonWasDown = false;
    bool mMouseDragging = false;
    bool mKeyWasDown = false;
    
    struct MousePosition
    {
        uint x;
        uint y;
    }mLastMousePosition;
};

inline void Application::SetAnimationRefreshRate(uint rate) { mAnimationRefreshRate = rate; }

}

#endif // APPLICATION_H
