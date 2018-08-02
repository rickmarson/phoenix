/*
 
 Third Person Camera
 
 */

#ifndef CAMERATHIRD_H
#define CAMERATHIRD_H

#include "camera_node.h"

namespace NYX {

class NYX_EXPORT CameraTP : public CameraNode
{
public:

    CameraTP(SceneNode *parent, IRenderer *renderer, std::string name);
    virtual ~CameraTP();

	void RotateCamera(float angle, eAxis eRotAxis);
	void RotateCamera(Quaternion rot);
	void TranslateCamera(Vector3 delta);

private:

	float rotAngX;
	float rotAngY;
};

}

#endif // CAMERATHIRD_H
