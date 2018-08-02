/*
 UI Events
*/

#ifndef UI_EVENTS_H
#define UI_EVENTS_H

#if PLATFORM_MAC
#include <SDL2/SDL_keycode.h>
#else
#include "SDL/SDL_keycode.h"
#endif

enum class UI_EVENT_ID
{
    UI_MOUSE_MOVED,
    UI_MOUSE_BUTTON_CLICKED,
    UI_MOUSE_WHEEL_MOVED,
    UI_MOUSE_DRAGGED,
    UI_KEY_PRESSED
};

struct UIEvent
{
    UIEvent( UI_EVENT_ID id_in ) : id(id_in) {}
    UI_EVENT_ID id;
};

struct UIMouseMovedEvent : public UIEvent
{
    UIMouseMovedEvent() : UIEvent( UI_EVENT_ID::UI_MOUSE_MOVED ) {}
    uint x;
    uint y;
};

struct UIMouseButtonEvent : public UIEvent
{
    UIMouseButtonEvent() : UIEvent( UI_EVENT_ID::UI_MOUSE_BUTTON_CLICKED ) {}
    uint button;
    uint x;
    uint y;
};

struct UIMouseDragEvent : public UIEvent
{
    UIMouseDragEvent() : UIEvent( UI_EVENT_ID::UI_MOUSE_DRAGGED ) {}
    uint button;
    int xrel;
    int yrel;
};

struct UIWheelMovedEvent : public UIEvent
{
    UIWheelMovedEvent() : UIEvent( UI_EVENT_ID::UI_MOUSE_WHEEL_MOVED ) {}
    int delta;
    uint x;
    uint y;
};

struct UIKeyPressedEvent : public UIEvent
{
    UIKeyPressedEvent() : UIEvent( UI_EVENT_ID::UI_KEY_PRESSED ) {}
    SDL_Keycode key;
};


#endif //UI_EVENTS_H
