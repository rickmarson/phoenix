/*
 
 Open GL Renderer Backend
 
 */

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "irenderer.h"

#if PLATFORM_MAC
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#else
#include "SDL.h"
#include "SDL_syswm.h"
#endif

#include "log_manager.h"
#include "mesh.h"
#include "Math/matrix4x4.h"

#include <map>

using namespace std;

#define GL_MAX_VISIBLE_LIGHTS 8

namespace NYX {

// Required Classes
class GLSLEffect;
class Texture;
class VertexBuffer;
class IndexBuffer;
class VertexArrayObject;
class RenderBuffer;
    
class NYX_EXPORT GLRenderer : public IRenderer
{
public:

	GLRenderer(SDL_Window *win, SDL_SysWMinfo winInfo);
    ~GLRenderer();

	void SetViewport(int x, int y, int w, int h) override;
    void GetViewport(int &x, int &y, int &w, int &h) override;
	void ClearScreen(float *clearColor = NULL) override;
	void SwapBuffers( void ) override;
    void EnableDepthTest( void ) override;
    void DisableDepthTest( void ) override;
    void EnableMSAA( void  ) override;
    void DisableMSAA( void ) override;
	void EnableCulling( void ) override;
	void DisableCulling( void ) override;
	void CullFrontFace( void ) override;
	void CullBackFace( void ) override;
	void SetFrontFaceToCCW( void ) override;
	void SetFrontFaceToCW( void ) override;
    void EnableAlphaBlending( void ) override;
    void DisableAlphaBlending( void ) override;
    void EnableMaxAlphaBlending( void ) override;
    
    virtual cl_context InitialiseCLContextFromGLContext( cl_device_id *device_id, cl_platform_id platfrom_id, cl_device_id cpu_fallback, int& error_code ) override;
    
	//************
	//deprecated: to be deleted
	void SetActiveCamera(Matrix4x4 &viewMatrix, Matrix4x4 &projectionMatrix,
							float clipNear, float clipFar, float fov);
	//***********

    Texture* CreateTexture( void ) override;
	void BindTexture(Texture* texture) override;
	void UnbindAllTextures() override;
    int GetActiveTexStackDepth() override;
    
	Effect* CreateShaderProgram(std::string shaderName) override;
	std::list<ShaderUniform>& GetShaderUniforms(void) override;
	void LoadShaderUniforms( void ) override;
	void UseShader(Effect* shader) override;

    VertexBuffer* CreateVertexBuffer( void ) override;
    IndexBuffer* CreateIndexBuffer( void ) override;
    VertexArrayObject* CreateVertexArrayObject( void ) override;
    RenderBuffer* CreateRenderBuffer( void ) override;
    
	//Dummies needed to implement the interface, they are only used in D3D renderers.
	//Might think of a use for them sometime.
	void BeginScene( void ) override {}
	void EndScene( void ) override {}
	//--------------

private:

	void DrawVertexBufferObject(MeshPtr mesh);
    
    typedef std::map<std::string, GLSLEffect*> EffectsMap;

	struct Viewport
	{
		int width;
		int height;
		int x;
		int y;
	}mCurrentViewport;

	SDL_GLContext mContext;
	SDL_Window *mSDLWindow;
	SDL_SysWMinfo mSDLWindowInfo;

	EffectsMap mEffects;
	GLSLEffect *mActiveEffect;

	//delete
	Matrix4x4 projMatrix;
	Matrix4x4 viewMatrix;

	uint mTextureStackDepth;
};

}

#endif // GLRENDERER_H
