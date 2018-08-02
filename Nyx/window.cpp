/*

Window

*/

#include "render_factory.h"
#include "window.h"

namespace NYX {

Window::Window(unsigned int w, unsigned int h, std::string title)
{
	mWidth = w;
	mHeight = h;
	mTitle = title;
	mDisplay = NULL;
}

Window::~Window()
{
	/*mActiveState.reset();
	for (auto it = mWindowStates.begin(); it != mWindowStates.end(); ++it)
	{
		(*it).reset(<#_Yp *__p#>)
	}*/
    
	delete mRenderEngine;
	SDL_DestroyWindow(mDisplay);
    TTF_Quit();
    SDL_Quit();
}

bool Window::InitDisplay(eRenderer backend, unsigned short bitDepth)
{
	mBitDepth = bitDepth;

	//TODO: add debug information
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
		return false;
	
    if ( TTF_Init() < 0 )
        return false;
    
	//window opengl gets overridden by d3d?
	if ( ( mDisplay = SDL_CreateWindow( mTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight, SDL_WINDOW_OPENGL ) ) == NULL )
		return false;

	static SDL_SysWMinfo winInfo;
	SDL_GetVersion(&winInfo.version);
	SDL_GetWindowWMInfo(mDisplay, &winInfo);

	if ( (mRenderEngine = CreateRenderer(mDisplay, winInfo, backend) ) == NULL )
		return false;

	mRenderEngine->SetViewport(0, 0, mWidth, mHeight);

	//initialize CL context
	mFxManager.Initialize(mDisplay, winInfo, mRenderEngine);

	return true;
}

void Window::Show()
{
	if (mDisplay)
		SDL_ShowWindow(mDisplay);
	//else print debug info
}

void Window::RenderFrame(bool updateAnimation)
{
	mRenderEngine->SetViewport(0, 0, mWidth, mHeight);
	
	mRenderEngine->ClearScreen(NULL);
	
    // updates UI if necessary
    mActiveState->UpdateUI();
    
	//process active state
	mActiveState->Render(updateAnimation);
    
	mRenderEngine->SwapBuffers();
}

void Window::AddWindowState(WindowStatePtr state) 
{
	mWindowStates.insert( state );
	if (state->IsActive())
		mActiveState = state;
}

WindowStatePtr Window::FindWindowState(std::string key) 
{
    WindowStatePtr state;

    for ( auto iter = mWindowStates.begin(); iter != mWindowStates.end(); iter++ )
    {
        if ( (*iter)->GetName() == key )
        {
            state = (*iter);
            break;
        }
    }
    
    return state;
}

bool Window::SetActiveState(std::string key)
{
	WindowStatePtr state = FindWindowState(key);
	if (!state)
		return false;

	state->SetActive(true);
	
	if (state->IsActive())
		mActiveState = state;

	return true;
}
    
void Window::SetNextState( void )
{
    auto current_state = mWindowStates.find( mActiveState );
    auto end = mWindowStates.end();
    
    if ( current_state == end )
        return; // something went wrong
    
    current_state++;
    
    if ( current_state == end )
    {
        // we are at the last state, loop around
        current_state = mWindowStates.begin();
    }
    
    mActiveState->SetActive( false );
    
    (*current_state)->SetActive(true);
    mActiveState = (*current_state);
}

void Window::SetPreviousState( void )
{
    auto current_state = mWindowStates.find( mActiveState );
    auto end = mWindowStates.end();
    
    if ( current_state == end )
        return; // something went wrong
    
    if ( current_state == mWindowStates.begin() )
    {
        // we are already at the first state, loop around
        current_state = mWindowStates.end();
        --current_state;
    }
    else
    {
        --current_state;
    }
    
    mActiveState->SetActive( false );
    
    (*current_state)->SetActive(true);
    mActiveState = (*current_state);
}
    
void Window::HandleUIEvent( UIEvent& event )
{
    switch ( event.id )
    {
        case UI_EVENT_ID::UI_KEY_PRESSED:
            if ( ((UIKeyPressedEvent&)event).key == SDLK_F1 )
            {
                SetNextState();
            }
            else if ( ((UIKeyPressedEvent&)event).key == SDLK_F2 )
            {
                SetPreviousState();
            }
            else
            {
                mActiveState->HandleUIEvent(event);
            }
            break;
        default:
            mActiveState->HandleUIEvent(event);
            break;
    }
}

}