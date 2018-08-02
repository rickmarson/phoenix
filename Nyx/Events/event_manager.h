#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

/*

Event Manager

*/

#include "events.h"
#include "log_manager.h"

//This event-based system is largely based on the book "Game Coding Complete, 3rd Edition"
//and the Qt SDK documentation

/*global typedefs, visible by all other subsystems including eventmanager.h
shared_ptr is needed here beacause:
- For the event system to work in an elegant way, the events must be queued
    as pointers to interfaces
- Events are created elsewhere in local scopes and in the majority of cases
    their pointers go out of scope long before the queue is processed
- shared_ptr allows to store these references in a safe way until they are
    processed and ensures they get desroyed only when they are no longer needed
- The same is true for the lists of listeners. If a pointer to an instance
    of a listener which "lives" elsewhere is simply added to the list it cannot be
    removed or the object's destructor will be called. the reference counting feature
    of shared_ptr allows multiple objetcs to point to the same instance without
    the risk of one of them destroying it prematurely.
*/

namespace NYX {

class BaseEventListener : public IEventListener
{
public:

    BaseEventListener(string name);
    virtual ~BaseEventListener();

    virtual string const GetName();
    virtual bool ProcessEvent(EventPtr event);

protected:

    string mName;

};

//the event manager is a singleton within the application for easy access
//anywhere.
class EventManager : public SingletonClass<EventManager>
{
public:

    EventManager();
    virtual ~EventManager();

    //queue an event for later (asyncronous) processing. Used for the
    //majoruty of messages.
    void QueueEvent(EventPtr event);
    //send and event instantanously (sincronous processing).
    //useful for urgent messages.
    bool SendEvent(EventPtr event);
    //register a listener to a certain type of event. That listener will be notified
    //each time an event of that type is raised.
    bool RegisterListener(eEventType eventType, EventListenerPtr listener);
    //a listener is tied to a game subsystem, therefore it's unregistered only when the subsystem
    //is shutting down and hence it gets unsuscribed to all the events it was listening for.
    bool UnregisterListener(EventListenerPtr listener);
    //sends all the event queued for processing in the prevoius loop.
    //this function gets called by the main loop.
    void ProcessEventQueue();
    //the time allocated to the event manager for event processing is limited.
    //this function resets the maximum time allowed.
    void ResetTimeoutThreshold(float maxTime);

private:

    //local typedefs visible only to the event manager
    typedef set< eEventType > EventTypeSet;
    typedef list< EventListenerPtr > ListenerList;
    typedef map< eEventType, ListenerList > ListenerMap;
    typedef list< EventPtr > EventQueue;

    EventTypeSet mRegisteredEvents;
    ListenerMap mRegisteredListeners;
    EventQueue mPendingEvents[2];

    float mMaxProcessingTime;

    //the event queue is double buffered to avoid an event being added while
    //the queue is being processed. newly created events go into an inactive queue
    //and are processed the next time around.
    unsigned short mActiveQueue;

    //swaps the active/inactive queues.
    inline void SwapQueues();

    //broadcast a single event. called both by sendEvent() and processEventQueue()
    inline bool ProcessEvent(EventPtr event);
};

}

#endif // EVENTMANAGER_H
