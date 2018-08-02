/*

A Space Scene

*/

#ifndef SPACE_SCENE_H
#define SPACE_SCENE_H

#pragma once 

#include "scene.h"

class SpaceScene : public NYX::Scene
{
    typedef NYX::Scene super;
    
public:

    SpaceScene(NYX::ApplicationPtr application);
	virtual ~SpaceScene();

	virtual void HandleUIEvent(UIEvent &event) override;
};

#endif // SPACE_SCENE_H
