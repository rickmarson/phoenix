/*

Event Iterfaces

*/

#ifndef IEVENT_H
#define IEVENT_H

#include <memory>

// interface list
namespace NYX {

class IEvent;
class IEventListener;

// global typedefs

typedef std::shared_ptr< IEvent > EventPtr;
typedef std::shared_ptr< IEventListener > EventListenerPtr;

//event types
enum eEventType
{
    EV_KEY_PRESS = 1001,
    EV_CAMERA_MOVED,
    EV_ACTIVE_CAMERA_CHANGED,
    EV_START_GAME_REQUESTED,
    EV_GAME_ENDED,
    EV_QUIT_APPLICATION,
    EV_OBJECT_MOVED
};

class NYX_EXPORT IEvent
{
public:
    virtual ~IEvent() {}
    virtual eEventType GetEventType() = 0; //returns the event type ID
    virtual std::string GetEventTypeStr() = 0; //returns a human-readable string
    //with the event type ID name. mainly for debug purposes.
};

class NYX_EXPORT IEventListener
{
public:
    virtual ~IEventListener() {}
    virtual std::string const GetName() = 0;
    virtual bool ProcessEvent(EventPtr event) = 0;
};

};

#endif // IEVENT_H
