/*
 
 A Physics Test
 
 */

#include "physics_test.h"
#include "camera_tp.h"
#include "camera_fp.h"
#include "Physics/rigid_body.h"

using namespace NYX;

PhysicsTest::PhysicsTest(ApplicationPtr application) :
	Scene(application)
{

}

PhysicsTest::~PhysicsTest()
{

}

void PhysicsTest::HandleUIEvent(UIEvent &event)
{
    switch ( event.id )
    {
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
