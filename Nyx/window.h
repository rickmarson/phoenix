/*
 
Window

*/

#ifndef WINDOW_H
#define WINDOW_H

#if PLATFORM_MAC
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>
#else
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_syswm.h"
#endif

#include "window_state.h"
#include "fx_manager.h"

#include <set>

namespace NYX {

class NYX_EXPORT Window
{
public:

    Window(unsigned int w, unsigned int h, std::string title = "");
    ~Window();

	float GetAspectRatio() { return ( (float)mWidth / (float)mHeight ); }
	uint GetWidth() { return mWidth; }
	uint GetHeight() { return mHeight; }

	bool InitDisplay(eRenderer backend, unsigned short bitDepth);
	void Show();
	void RenderFrame(bool updateAnimation); //update

	IRenderer* GetRenderer() {return mRenderEngine;}

	void AddWindowState(WindowStatePtr state);
	WindowStatePtr FindWindowState(std::string key);
    WindowStatePtr GetActiveState( void )               { return mActiveState; }
	bool SetActiveState(std::string key);
    void SetNextState( void );
    void SetPreviousState( void );
    
    void HandleUIEvent( UIEvent& event );
    
private:

	std::string mTitle;
	uint mWidth;
	uint mHeight; 
	uint mBitDepth;
	bool fullscreen;

	SDL_Window *mDisplay;
	IRenderer *mRenderEngine;
	FXManager  mFxManager;

	std::set<WindowStatePtr> mWindowStates;
	WindowStatePtr mActiveState;
};

}

#endif // WINDOW_H
