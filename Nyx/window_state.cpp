/*

Window State

*/

#include "window_state.h"
#include "window.h"
#include "scene.h"
#include "UI/ui_manager.h"

namespace NYX {

WindowState::WindowState( Window *parent, std::string name, Scene* scene ) :
	mParentWindow( parent ),
	mName( name ),
    mScene( scene ),
	mSceneRoot( new RootNode(parent->GetRenderer(), "Root") ),
    mUIManager( new UIManager() )
	{
        mUIManager->Initialise(parent->GetRenderer());
    
    }

WindowState::~WindowState()
{
    mParentWindow = NULL;
    mSceneRoot.reset();
    delete mUIManager;
}
    
void WindowState::Render(bool updateAnimation)
{
	if(!mSceneRoot) //failsafe
		return;
	
    // render 3d scene
	mSceneRoot->UpdateAnimation(updateAnimation);
	mSceneRoot->ProcessNode();
    
    // render UI
    mUIManager->Render();
    
}
    
void WindowState::UpdateUI( void )
{
    // regenerates the UI texture
    mUIManager->UpdateUI();
}

void WindowState::HandleUIEvent( UIEvent& event )
{
    if ( !mUIManager->HandleUIEvent(event) )
        mScene->HandleUIEvent(event);
    
    
}

}