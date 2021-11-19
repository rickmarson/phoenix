/*

Application Class

*/

#include "application.h"
#include "scene.h"
#include "UI/ui_events.h"
#include "camera_fp.h"
#include "camera_tp.h"

#include "json.hpp"

using nlohmann::json;

namespace NYX {

const std::string CONFIG_EXTENSION = ".json";
    
Application::Application(char* argv0, std::string logFileName) :
        pFileManager( new FileManager(argv0) ),
		pLogger( new LogManager(logFileName) ),
        pResourceCache( new ResourceCache() ),
		pEventMng( new EventManager() ),
		pScriptManager( new ScriptManager() ),
		mLogFile(logFileName),
		pArgv0(argv0),
		bIsRunning(false),
		mMouseSensitivity(5), //assign a sensible default value so that camera movement doesn't explode
		mAnimationRefreshRate(20)
{
}

Application::~Application()
{
	pResourceCache.reset();
	pWindow.reset();
	pScriptManager.reset();
	pEventMng.reset();
	pLogger.reset();
}

void Application::Initialise( string config_file )
{
    LoadConfig( config_file );
    
    CreateGameWindow(mWindowProps.title, mWindowProps.width, mWindowProps.height, mWindowProps.depth, mWindowProps.backend );
}
    
void Application::CreateGameWindow(std::string name, int w, int h, int depth, NYX::eRenderer eBackend)
{ 
	//attempt to create the game window
	WindowPtr win( new Window(w, h, name) );
	//end program if unsuccessfull
	assert(win->InitDisplay(eBackend, depth));
	//store window
	pWindow = win;
	//pass the newly created renderer to the cache. It's needed to create textures.
	pResourceCache->SetRenderer(win->GetRenderer());
	//Initialize scripting engine
	pScriptManager->Init();
}

void Application::RegisterCacheSearchPath(std::string mountPoint, std::string path)
{
	pResourceCache->RegisterSearchPath(mountPoint, path);
}
    
void Application::ShowWindow(bool show)
{
	if (show)
		pWindow->Show();

	//else hide....
}
    
void Application::Run( void )
{
    // run game loop
    bIsRunning = true;
    SDL_Event ui_event;
    mLoopStartTime = mCurTime = SDL_GetTicks();
    
    //main loop
    while ( bIsRunning )
    {
        SDL_PollEvent(&ui_event);
        
        // preprocess certain ui events before piping them to the game
        switch (ui_event.type)
        {
            case SDL_WINDOWEVENT:
                switch (ui_event.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    bIsRunning = false;
                    break;
                }
                break;
            case SDL_KEYDOWN:
                mKeyWasDown = true;
                break;
            case SDL_KEYUP:
                if ( mKeyWasDown )
                {
                    UIKeyPressedEvent key_event;
                    key_event.key = ui_event.key.keysym.sym;
                    
                    pWindow->HandleUIEvent(key_event);
                    
                    mKeyWasDown = false;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                mMouseButtonWasDown = true;
                break;
            case SDL_MOUSEBUTTONUP:
                
                if ( !mMouseDragging && mMouseButtonWasDown )
                {
                    UIMouseButtonEvent click_event;
                    click_event.button = ui_event.button.state;
                    click_event.x = ui_event.button.x;
                    click_event.y = ui_event.button.y;
                    mMouseButtonWasDown = false;
                    
                    pWindow->HandleUIEvent(click_event);
                }
                else if ( mMouseDragging && mMouseButtonWasDown )
                {
                    mMouseButtonWasDown = false;
                    mMouseDragging = false;
                }
                
                break;
            case SDL_MOUSEMOTION:
                mLastMousePosition.x = ui_event.motion.x;
                mLastMousePosition.y = ui_event.motion.y;
                
                if ( mMouseButtonWasDown && !mMouseDragging )
                {
                    mMouseDragging = true;
                }
                else if ( mMouseButtonWasDown && mMouseDragging )
                {
                    UIMouseDragEvent drag_event;
                    drag_event.button = ui_event.motion.state;
                    drag_event.xrel = ui_event.motion.xrel;
                    drag_event.yrel = ui_event.motion.yrel;
                    
                    pWindow->HandleUIEvent(drag_event);
                }
                else
                {
                    UIMouseMovedEvent mouse_event;
                    mouse_event.x = ui_event.motion.x;
                    mouse_event.y = ui_event.motion.y;
                    
                    pWindow->HandleUIEvent(mouse_event);
                }

                break;
            case SDL_MOUSEWHEEL:
            {
                UIWheelMovedEvent wheel_event;
                wheel_event.delta = ui_event.wheel.y;
                wheel_event.x = mLastMousePosition.x;
                wheel_event.y = mLastMousePosition.y;
                pWindow->HandleUIEvent(wheel_event);
            }
                break;
            default:
                break;
        }
        
        pWindow->GetActiveState()->GetScene()->Update();
    }
}
    
void Application::LoadConfig( std::string file_name )
{
    char* config_raw = ResourceCache::GetInstance()->RequestSourceCode(file_name + CONFIG_EXTENSION);
    
    auto config_source = json::parse(config_raw);
    
    if (config_source.empty())
    {
        //log message
        return;
    }
    
    // read window config options
    const auto& window = config_source["window"];

    mWindowProps.title = window["title"].get<std::string>();
    mWindowProps.width = window["width"].get<int>();
    mWindowProps.height = window["height"].get<int>();
    mWindowProps.depth = window["depth"].get<int>();
    std::string backend_str = window["backend"].get<std::string>();
    
    if ( backend_str == "OpenGL" )
    {
        mWindowProps.backend = NYX::OPENGL;
    }
    else if ( backend_str == "DirectX" )
    {
        mWindowProps.backend = NYX::D3D;
    }
}

}
