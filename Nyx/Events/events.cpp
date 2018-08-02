/*

Events

*/

#include "events.h"
#include <vector>

using namespace std;

namespace NYX {

BaseEvent::BaseEvent(eEventType eventType)
{
    mEventType = eventType;
}

BaseEvent::~BaseEvent()
{

}

eEventType BaseEvent::GetEventType()
{
    return mEventType;
}

string BaseEvent::GetEventTypeStr()
{
    string ev_type_name;

    switch (mEventType)
    {
    case EV_KEY_PRESS:
        ev_type_name = "KEY PRESSED";
        break;
    case EV_CAMERA_MOVED:
        ev_type_name = "CAMERA MOVED";
        break;
    case EV_START_GAME_REQUESTED:
        ev_type_name = "START GAME REQUESTED";
        break;
    case EV_GAME_ENDED:
        ev_type_name = "GAME ENDED";
    case EV_QUIT_APPLICATION:
        ev_type_name = "QUIT GAME";
        break;
    }

    return ev_type_name;
}

KeyEvent::KeyEvent(int whichKey) :
    BaseEvent(EV_KEY_PRESS)
{
    mKey = whichKey;
}

KeyEvent::~KeyEvent()
{

}

int KeyEvent::Key()
{
    return mKey;
}

CameraMovedEvent::CameraMovedEvent(Vector3 pos) :
    BaseEvent(EV_CAMERA_MOVED)
{
    mNewPosition = pos;
	bIsRot = false;
	bUseQuaternion = false;
}

CameraMovedEvent::CameraMovedEvent(float angle, eAxis axis) :
    BaseEvent(EV_CAMERA_MOVED)
{
	mRotAngle = angle;
	eRotAxis = axis;
	bIsRot = true;
	bUseQuaternion = false;
}

CameraMovedEvent::CameraMovedEvent(Quaternion rot) :
    BaseEvent(EV_CAMERA_MOVED)
{
	mRot = rot;
	bIsRot = true;
	bUseQuaternion = true;
}

CameraMovedEvent::~CameraMovedEvent()
{

}

Vector3 CameraMovedEvent::Position()
{
    return mNewPosition;
}

float CameraMovedEvent::RotAngle()
{
	return mRotAngle;
}

eAxis CameraMovedEvent::RotAxis()
{
	return eRotAxis;
}

Quaternion CameraMovedEvent::Rotation()
{
	return mRot;
}

ObjectMovedEvent::ObjectMovedEvent(uint id, Vector3 pos, Matrix3x3 attitude) :
    BaseEvent(EV_OBJECT_MOVED),
	mMesh(NULL),
	bUseQuaternion(false),
	bIsSoft(false)
{
	mObjectID = id;
	mNewPosition = pos;
	mAttitude = attitude;
}

ObjectMovedEvent::ObjectMovedEvent(uint id, Vector3 pos, Quaternion rot) :
    BaseEvent(EV_OBJECT_MOVED),
	mMesh(NULL),
	bUseQuaternion(true),
	bIsSoft(false)
{
	mObjectID = id;
	mNewPosition = pos;
	mRot = rot;
}

ObjectMovedEvent::~ObjectMovedEvent()
{
	delete mMesh;
}

uint ObjectMovedEvent::ID()
{
	return mObjectID;
}
 
Vector3 ObjectMovedEvent::Position()
{
	return mNewPosition;
}

Matrix3x3 ObjectMovedEvent::AttitudeMatrix()
{
	return mAttitude;
}

void ObjectMovedEvent::AddSoftBodyMesh(float* mesh)
{
	mMesh = mesh;
	bIsSoft = true;
}

float* ObjectMovedEvent::GetMesh()
{
	return mMesh;
}

Quaternion ObjectMovedEvent::Rotation()
{
	return mRot;
}

GameEndedEvent::GameEndedEvent(bool won) :
    BaseEvent(EV_GAME_ENDED)
{
    mGameIsWon = won;
}

GameEndedEvent::~GameEndedEvent()
{

}

bool GameEndedEvent::GameIsWon()
{
    return mGameIsWon;
}

}
