#ifndef EVENTS_H
#define EVENTS_H

/*

Events

*/

#include <list>
#include <utility>
#include <set>
#include <map>

#include "ievent.h"
#include "events.h"
#include "Math/constants.h"
#include "Math/vector3.h"
#include "Math/matrix3x3.h"
#include "Math/quaternion.h"

namespace NYX {

class BaseEvent : public IEvent
{
public:

    BaseEvent(eEventType eventType);
    virtual ~BaseEvent();
    virtual eEventType GetEventType();
    virtual std::string GetEventTypeStr();

protected:

    eEventType mEventType;

};


class KeyEvent : public BaseEvent
{
public:

    KeyEvent(int whichKey);
    virtual ~KeyEvent();

    int Key();

private:

    int mKey;
};

class CameraChangedEvent : public BaseEvent
{
public:

	CameraChangedEvent(int id) : 
		BaseEvent(EV_ACTIVE_CAMERA_CHANGED),
		mId(id)
	{}

	virtual ~CameraChangedEvent() {}

	int Id() {return mId;}

private:

    int mId;
};

class CameraMovedEvent : public BaseEvent
{
public:

    CameraMovedEvent(Vector3 pos);
	CameraMovedEvent(float angle, eAxis axis);
	CameraMovedEvent(Quaternion rot);
    virtual ~CameraMovedEvent();

    Vector3 Position();
	float RotAngle();
	eAxis RotAxis();
	Quaternion Rotation();

	bool bIsRot;
	bool bUseQuaternion;

private:

	float mRotAngle;
	eAxis eRotAxis;
	Quaternion mRot;
    Vector3 mNewPosition;
};

class ObjectMovedEvent : public BaseEvent
{
public:

    ObjectMovedEvent(uint id, Vector3 pos, Matrix3x3 attitude);
	ObjectMovedEvent(uint id, Vector3 pos, Quaternion rot);
    virtual ~ObjectMovedEvent();

	uint ID();
    Vector3 Position();
	Matrix3x3 AttitudeMatrix();
	Quaternion Rotation();

	void AddSoftBodyMesh(float* mesh);
	float* GetMesh();

	bool bUseQuaternion;
	bool bIsSoft;

private:

	uint mObjectID;
	Quaternion mRot;
	Matrix3x3 mAttitude;
    Vector3 mNewPosition;
	float* mMesh;
};

class GameEndedEvent : public BaseEvent
{
public:
    GameEndedEvent(bool won);
    virtual ~GameEndedEvent();

    bool GameIsWon();

private:
    bool mGameIsWon;

};

}

#endif // EVENTS_H
