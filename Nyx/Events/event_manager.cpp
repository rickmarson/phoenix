
/*
 
 Event Manager
 
 */

#include "event_manager.h"

#include <ctime>

namespace NYX {

BaseEventListener::BaseEventListener(string name)
{
    mName = name;
}

BaseEventListener::~BaseEventListener()
{

}

string const BaseEventListener::GetName()
{
    return mName;
}

bool BaseEventListener::ProcessEvent(EventPtr event)
{
    //Listens for all events and logs their types
#ifdef __DEBUG__
        string evT = event->GetEventTypeStr();
        string msg = "Event " + evT +" received by listener: " + mName;

        LogManager::GetInstance()->LogMessage(msg.c_str());
#endif

    return true;
}

EventManager::EventManager()
{
    LogManager::GetInstance()->LogMessage("Initialising Event Manager...");

    mActiveQueue = 0;
    mMaxProcessingTime = 0.01; //10 milli-seconds.
}

EventManager::~EventManager()
{
    LogManager::GetInstance()->LogMessage("Event Manager Shutting Down.");
}

void EventManager::SwapQueues()
{
    if (mActiveQueue == 0)
        mActiveQueue = 1;
    else
        mActiveQueue = 0;
}

void EventManager::ResetTimeoutThreshold(float maxTime)
{
    mMaxProcessingTime = maxTime;
}

bool EventManager::RegisterListener(eEventType eventType, EventListenerPtr listener)
{
    /*insert the requested event type in the set of registered events.
    if ret.second == false, insertion failed because the requested
    eventType is already present.*/
    pair<set<eEventType>::iterator,bool> reg_evt_ret;
    pair<ListenerMap::iterator,bool> reg_lis_ret;

    reg_evt_ret = mRegisteredEvents.insert(eventType);

    //Check if a list of listeners associated with the requested eventType
    //is already present.
    ListenerMap::iterator it;

    it = mRegisteredListeners.find(eventType);

    if (it == mRegisteredListeners.end())
    {
        //eventType not already mapped, create an entry and related (empty) list
        reg_lis_ret = mRegisteredListeners.insert( pair<eEventType, ListenerList>(eventType, ListenerList() ) );

        //log an error message and return false if creation fails.
        if (reg_lis_ret.second == false)
        {
            LogManager::GetInstance()->LogMessage("Event Manager Error! Failed to Create Event Listeners List.");
            return false;
        }

        it = reg_lis_ret.first;
    }

    ListenerList & tmpList = (*it).second;

    //if the listener is already registered to the requested event, return false
    for (ListenerList::iterator it2 = tmpList.begin(); it2 != tmpList.end(); ++it2)
    {
        if ( *it2 == listener )
            return false;
    }

    //register the listener to the requested event
    tmpList.push_back( listener );

    return true;

}

bool EventManager::UnregisterListener(EventListenerPtr listener)
{
    bool ret = false;

    //for each event type, search for a registered listener that matches the requested one
    for (ListenerMap::iterator m_it = mRegisteredListeners.begin(); m_it != mRegisteredListeners.end(); ++m_it)
    {
        ListenerList & regList = (*m_it).second;

        for (ListenerList::iterator l_it = regList.begin(); l_it != regList.end(); ++l_it)
        {
            //if one is
            if ( (*l_it) == listener )
            {
                regList.erase( l_it );

                ret = true;
                break;
            }
        }
    }

    return ret;
}

bool EventManager::ProcessEvent(EventPtr event)
{
    eEventType evT = event->GetEventType();

    //Validate Event
    EventTypeSet::iterator it_set;

    it_set = mRegisteredEvents.find(evT);

    //the event is not a registered event, log a warning message and return false
    if (it_set == mRegisteredEvents.end())
    {
        string msg = "Event System Warning: An Unregistered Event Has Been Requested! Event: " + event->GetEventTypeStr();

        LogManager::GetInstance()->LogMessage(msg.c_str());

        return false;
    }

    ListenerMap::iterator it_map;

    it_map = mRegisteredListeners.find(evT);

    //no registered listeners found for this event, return false
    if (it_map == mRegisteredListeners.end())
        return false;

    //get the list of registered listeners
    ListenerList & lstList = (*it_map).second;

    //iterate through all listeners and send them the event
    for (ListenerList::iterator it_lst = lstList.begin(); it_lst != lstList.end(); ++it_lst)
    {
        EventListenerPtr listener = (*it_lst);

        bool processed;

        processed = listener->ProcessEvent(event);
    }

    return true;
}

void EventManager::QueueEvent(EventPtr event)
{
    unsigned short insertQueue;

    /*Queue the newly created event in the inactive queue.
      Queues are swapped each time the currently active cue is empty (usually at the end of every main loop)
    */
    if (mActiveQueue == 0)
        insertQueue = 1;
    else
        insertQueue = 0;

    mPendingEvents[insertQueue].push_back(event);

}

void EventManager::ProcessEventQueue()
{
    clock_t startTime, time;
    float dt = 0.0; //elapsed time in seconds.

    EventQueue & activeQueue = mPendingEvents[mActiveQueue];

    startTime = clock();

    /*the time allowed to process the event queue is limited during each main loop, to avoid lagging
      when too many events are spawned. The remaining events, if any, are processed in the next loop.
      The timeout is set by default at 1/4 of the main loop time.*/
    while (dt <= mMaxProcessingTime)
    {
        EventQueue::iterator q_it;
        q_it = activeQueue.begin();

        //if the queue is empty, all pending events have been processed in less than the max allowed time.
        if (q_it == activeQueue.end())
            break;

        //copy the first event in the queue
        EventPtr event = (*q_it);
        //delete the first event in the queue
        activeQueue.pop_front();

        bool processed;

        processed = ProcessEvent(event);

        //The reference count for event is now 1. it will automatically drop to 0 when the variable event goes out
        //of scope at the end of the loop cycle, and the instance of the contained class will be automatically deleted.
        //Note: shared_ptr remembers the actual class it contains along with the correct deconstructor, even if the demplete
        //is declared with the parent interface.

        //might check if event was correctly processed.

        time = clock();
        dt = (time - startTime)/(float)CLOCKS_PER_SEC;
    }

    //if there are still pending events that could not be processed due to timeout,
    //the queues will not be swapped and the waiting events will be processed in 2 turns.
    if (activeQueue.empty())
        SwapQueues();
}


bool EventManager::SendEvent(EventPtr event)
{
    return ProcessEvent(event);
}

}
