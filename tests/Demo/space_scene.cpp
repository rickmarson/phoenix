/*
 
 A Space Scene
 
 */

#include "space_scene.h"
#include "camera_tp.h"
#include "camera_fp.h"
#include "Physics/rigid_body.h"

using namespace NYX;

SpaceScene::SpaceScene(ApplicationPtr application) :
	Scene(application)
{

}

SpaceScene::~SpaceScene()
{

}

void SpaceScene::HandleUIEvent(UIEvent &event)
{
    switch ( event.id )
    {
        case UI_EVENT_ID::UI_KEY_PRESSED:
            
            if ( ((UIKeyPressedEvent&)event).key == SDLK_1 )
            {
                //active camera: camera 1
                EventPtr event(new CameraChangedEvent( mCameras.front()->GetID() ));
                mEventMng->QueueEvent(event);
            }
            else if ( ((UIKeyPressedEvent&)event).key == SDLK_2 )
            {
                //active camera: camera 2
                EventPtr event(new CameraChangedEvent( mCameras.back()->GetID() ));
                mEventMng->QueueEvent(event);
            }
            break;
        case UI_EVENT_ID::UI_MOUSE_DRAGGED:
            
            if ( ((UIMouseDragEvent&)event).button & SDL_BUTTON_RMASK )
            {
                int xrel = ((UIMouseDragEvent&)event).xrel;
                int yrel = ((UIMouseDragEvent&)event).yrel;
                
                EventPtr event(new CameraMovedEvent(Vector3(xrel/mApplication->mMouseSensitivity,
                                                            yrel/mApplication->mMouseSensitivity,
                                                            0.0)));
                mEventMng->QueueEvent(event);
            }
            
            break;
        case UI_EVENT_ID::UI_MOUSE_WHEEL_MOVED:
        {
            int delta = ((UIWheelMovedEvent&)event).delta;
            
            EventPtr event(new CameraMovedEvent(Vector3(0.0,
                                                        0.0,
                                                        delta/100)));
            mEventMng->QueueEvent(event);
            break;
        }
        default:
            super::HandleUIEvent(event);
            break;
    }
}
    
