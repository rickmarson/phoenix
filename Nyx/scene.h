/*

Scene

*/

#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "window.h"
#include "application.h"
#include "Physics/physics_engine.h"

namespace NYX {

// Required Classes
typedef void* xml_node;
class IRenderer;
    
class Scene
{
public:

	Scene(NYX::ApplicationPtr application);
	virtual ~Scene();

	bool SetupScene(std::string scene_name);
	void Update( void );
	virtual void HandleUIEvent(UIEvent& event);

protected:

	typedef std::shared_ptr< NYX::IPhysicsEngine > PhysicsEnginePtr;

    std::string mName;
	NYX::ApplicationPtr mApplication;
    WindowPtr mWindow;
	PhysicsEnginePtr pPhysicsEngine;
	NYX::EventManager* mEventMng;
    IRenderer* mRenderer;
    
	//need to keep track of camera IDs in the current scene
	vector<CameraNode*> mCameras;
    
    // temporarily store targets for each object that has one
    map<SceneNodePtr, string> targets;
    
private:
    
};
    
typedef std::shared_ptr<Scene> ScenePtr;
    
}

#endif // SCENE_H
