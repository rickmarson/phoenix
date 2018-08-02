/*

First Person Camera

*/

#ifndef CAMERAFIRST_H
#define CAMERAFIRST_H

#include "camera_node.h"


namespace NYX {

class NYX_EXPORT CameraFP : public CameraNode
{
public:

    CameraFP(SceneNode *parent, IRenderer *renderer, std::string name);
    virtual ~CameraFP();

	void RotateCamera(float angle, eAxis eRotAxis);
	void RotateCamera(Quaternion rot);
	void TranslateCamera(Vector3 delta);

private:


};

}

#endif // CAMERAFIRST_H
