/*
 
 Particle Effect Node
 
 */


#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "scene_node.h"
#include "particlefx.h"
#include "material.h"

namespace NYX {

class NYX_EXPORT ParticleNode : public SceneNode
{
public:

    ParticleNode(SceneNode *parent, IRenderer *renderer, std::string name);
    ~ParticleNode();

	void UseScriptParticleGeneration(std::string source, std::string funcName);
	void InitializeFX(std::string programName, std::string kernelName, uint size, std::string materialName);
	virtual void ProcessNode();

protected:

	ParticleFX mFX;
};

inline void ParticleNode::UseScriptParticleGeneration(std::string source, std::string funcName) { mFX.UseScriptParticleGeneration(source, funcName); }

}

#endif // PARTICLENODE_H
