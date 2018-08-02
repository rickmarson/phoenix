/*

Window State

*/

#ifndef WINDOW_STATE_H
#define WINDOW_STATE_H

#include "root_node.h"
#include "UI/ui_events.h"

namespace NYX {

class Window;
class Scene;
class UIManager;

class NYX_EXPORT WindowState
{
    friend class Scene;
    
public:

    WindowState( Window *parent, std::string name, Scene* scene );
    ~WindowState();

	std::string GetName( void ) const           { return mName; }
    std::string GetTitle( void ) const          { return mTitle; }
    Scene* GetScene( void )                     { return mScene; }
	RootNodePtr GetRootNode( void )             { return mSceneRoot; }
	bool IsActive( void )                       { return mIsActive; }
	void SetActive(bool active)                 { mIsActive = active; }

	void AssignSceneGraph(RootNodePtr scene)    { mSceneRoot = scene; } //of doubtful usefulness

	void Render(bool updateAnimation);
    void UpdateUI( void );
    
    void HandleUIEvent( UIEvent& event );
    
private:

	std::string mName;
    std::string mTitle;
	Window *mParentWindow;
    Scene *mScene;
	RootNodePtr mSceneRoot;
    UIManager *mUIManager;
    
	bool mIsActive = false;
    bool mShowTitle = true;

};

typedef std::shared_ptr<WindowState> WindowStatePtr;

}

#endif // WINDOW_STATE_H
