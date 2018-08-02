/*

Interface for the Renderer

*/

#ifndef IRENDERER_H
#define IRENDERER_H

#include "shader_uniform.h"

#include <list>
#include <memory>

namespace NYX {

	enum eRenderer
	{
		OPENGL = 0x1,
		D3D
	};


    // Required classes
	class Mesh;
	class Model;
	class Material;
	class Matrix4x4;
    class Texture;
    class Effect;
    class VertexBuffer;
    class IndexBuffer;
    class VertexArrayObject;
    class RenderBuffer;

	typedef std::shared_ptr< Mesh > MeshPtr;
	typedef std::shared_ptr< Model > ModelPtr;
	typedef std::shared_ptr< Material > MaterialPtr;

    // Forward declare types for OpenCL interop
    typedef struct _cl_device_id*      cl_device_id;
    typedef struct _cl_context*        cl_context;
    typedef struct _cl_platform_id*    cl_platform_id;
    
	class NYX_EXPORT IRenderer
	{
	public:

		enum E_POLYGON_TYPE
		{
			E_POINT,
			E_LINE,
			E_TRIANGLE,
            E_TRIANGLE_STRIP,
			E_QUAD,
			E_POLYGON
		};

		virtual ~IRenderer() {}
		virtual void SetViewport(int x, int y, int w, int h) = 0;
        virtual void GetViewport(int &x, int &y, int &w, int &h) = 0;
		virtual void ClearScreen(float *clearColor = NULL) = 0;
		virtual void SwapBuffers( void ) = 0;
        virtual void EnableDepthTest( void ) = 0;
        virtual void DisableDepthTest( void ) = 0;
        virtual void EnableMSAA( void  ) = 0;
        virtual void DisableMSAA( void ) = 0;
		virtual void EnableCulling( void ) = 0;
		virtual void DisableCulling( void ) = 0;
		virtual void CullFrontFace( void ) = 0;
		virtual void CullBackFace( void ) = 0;
		virtual void SetFrontFaceToCCW( void ) = 0;
		virtual void SetFrontFaceToCW( void ) = 0;
        virtual void EnableAlphaBlending( void ) = 0;
        virtual void DisableAlphaBlending( void ) = 0;
        virtual void EnableMaxAlphaBlending( void ) = 0;
        
        virtual cl_context InitialiseCLContextFromGLContext( cl_device_id *device_id, cl_platform_id platfrom_id, cl_device_id cpu_fallback, int &error_code ) = 0;
        
		//**********
		//to be deleted
		virtual	void SetActiveCamera(Matrix4x4 &viewMatrix, Matrix4x4 &projectionMatrix,
			float clipNear, float clipFar, float fov) = 0;
		//***********

		//check types of size)
        virtual Texture* CreateTexture( void ) = 0;
        virtual void BindTexture(Texture* texture) = 0;
        virtual void UnbindAllTextures() = 0;
		virtual int GetActiveTexStackDepth() = 0;

		virtual Effect* CreateShaderProgram(std::string shaderName) = 0;
		virtual std::list<ShaderUniform>& GetShaderUniforms(void) = 0;
		virtual void LoadShaderUniforms(void) = 0;
		virtual void UseShader(Effect* shader) = 0;

        virtual VertexBuffer* CreateVertexBuffer( void ) = 0;
        virtual IndexBuffer* CreateIndexBuffer( void ) = 0;
        virtual VertexArrayObject* CreateVertexArrayObject( void ) = 0;
        virtual RenderBuffer* CreateRenderBuffer( void ) = 0;
        
		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;
	};

}

#endif // IRENDERER_H