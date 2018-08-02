/*

Open GL Renderer Backend

*/

#include "glsl_effect.h"

#if PLATFORM_MAC
#include <OpenCL/OpenCL.h>
#else
#include <CL/opencl.h>
#endif

#include "gl_renderer.h"
#include "resource_cache.h"
#include "gl_utils.h"
#include "gl_texture.h"
#include "gl_vertex_buffer.h"
#include "gl_index_buffer.h"
#include "gl_vertex_array_object.h"
#include "gl_render_buffer.h"



namespace NYX {

GLenum  cubemap[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                     GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                     GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

GLRenderer::GLRenderer(SDL_Window *win, SDL_SysWMinfo winInfo) :
	mTextureStackDepth(0),
	mActiveEffect(nullptr)
{
	mSDLWindow = win;
	mSDLWindowInfo = winInfo;

	SDL_GL_LoadLibrary(nullptr); //load system default

	//set attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
    
    // note: this is particularly important on Mac, where SDL reverts to the compatibility profile without it.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   
	//create context
	mContext = SDL_GL_CreateContext(mSDLWindow);
	SDL_GL_MakeCurrent(mSDLWindow, mContext);

	string glMsg;

    // even with the core profile 4.1, this appears to be necessary
    glewExperimental = GL_TRUE;
    
	GLenum error_code = glewInit();
	CheckGLError();
    if ( error_code != GL_NO_ERROR )
    {
        LogManager::GetInstance()->LogMessage((char*)glewGetErrorString(error_code));
    }
    
	glMsg = "Using GLEW Version: ";
	glMsg += (char*)glewGetString(GLEW_VERSION);
	LogManager::GetInstance()->LogMessage(glMsg.c_str());

	glMsg = "OpenGL Version: ";
	glMsg += (char*)glGetString(GL_VERSION);
	LogManager::GetInstance()->LogMessage(glMsg.c_str());
    
	glMsg = "GPU: ";
	glMsg += (char*)glGetString(GL_VENDOR);
	glMsg += " ";
	glMsg += (char*)glGetString(GL_RENDERER);
	LogManager::GetInstance()->LogMessage(glMsg.c_str());
    
	glMsg = "GLSL Version: ";
	glMsg += (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	LogManager::GetInstance()->LogMessage(glMsg.c_str());
    
#if DEBUG
	glMsg = "OpenGL Extensions Supported: ";
	glMsg += LINE_END + "\t\t\t";
    GLint n;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for ( GLint i = 0; i < n; i++)
    {
        glMsg += (char*)glGetStringi(GL_EXTENSIONS, i);
        glMsg += LINE_END + "\t\t\t";
    }
    
    LogManager::GetInstance()->LogMessage(glMsg.c_str());
#endif

	//need glDrawElementsBaseVertex supported in order to use VBO with our of index buffer setup. 
	bool VBOsAreSupported = ( glewIsSupported("GL_VERSION_3_2") ||
							glewIsSupported("GL_ARB_draw_elements_base_vertex") );

    // only use core open gl, from 3.2 up.
    ASSERT(VBOsAreSupported);
}

GLRenderer::~GLRenderer()
{
	for (EffectsMap::iterator fx_it = mEffects.begin(); fx_it != mEffects.end(); ++fx_it )
	{
		delete (*fx_it).second;
	}
	SDL_GL_DeleteContext(mContext);
	SDL_GL_UnloadLibrary();  
	mSDLWindow = nullptr;
	mActiveEffect = nullptr;
}

cl_context GLRenderer::InitialiseCLContextFromGLContext( cl_device_id *device_id, cl_platform_id platfrom_id, cl_device_id cpu_fallback, int& error_code )
{
    cl_context clContext = nullptr;
    
    //init CL context from current GL context
#if PLATFORM_WINDOWS
    HGLRC glContext = wglGetCurrentContext();
    HDC winDevice  = GetDC( mSDLWindowInfo.info.win.window );
    
    cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties)glContext,
        CL_WGL_HDC_KHR, (cl_context_properties)winDevice,
        CL_CONTEXT_PLATFORM, (cl_context_properties)platfrom_id,
        0
    };
    
    clContext = clCreateContext(properties, 1, (::cl_device_id*)device_id, NULL, NULL, &error_code);
    
#elif PLATFORM_MAC
    
    CGLContextObj gl_context = CGLGetCurrentContext();
    CGLShareGroupObj gpu_share_group = CGLGetShareGroup(gl_context);
    
    gcl_gl_set_sharegroup(gpu_share_group);
    
    cl_context_properties properties[] = {
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)gpu_share_group,
        CL_CONTEXT_PLATFORM, (cl_context_properties)platfrom_id,
        0
    };
    
    clContext = clCreateContext(properties, 1, device_id, NULL, NULL, &error_code);
    
    // on Macs, particularly Macbooks, it's common to have to GPUs and the integrated Intel one
    // might have been selected as the CL Device if it's the first one in the devices array. Command
    // queue creation will fail with the integrated GPU, so this code double checks mCLDevice is set to
    // the right one or overrides the selection here.
    dispatch_queue_t queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_GPU, NULL);
    cl_device_id gpu_id = gcl_get_device_id_with_dispatch_queue(queue);
    
    if ( gpu_id != nullptr && gpu_id != *device_id )
        *device_id = gpu_id;
    else if ( gpu_id == nullptr )
    {
        // if there is no suitable gpu, revert to cpu
        *device_id = cpu_fallback;
    }
    
#else
    LogManager::GetInstance()->LogMessage("OpenCL: Platform Not Supported.");
    exit(-1);
#endif
    
    return clContext;
}
    
void GLRenderer::SetViewport(int x, int y, int w, int h)
{
    SDL_GL_MakeCurrent(mSDLWindow, mContext);
    
	glViewport(x, y, w, h);
    
	mCurrentViewport.width = w;
	mCurrentViewport.height = h;
	mCurrentViewport.x = x;
	mCurrentViewport.y = y;
}
    
void GLRenderer::GetViewport(int &x, int &y, int &w, int &h)
{
    GLint current_viewport[4];
    glGetIntegerv(GL_VIEWPORT, current_viewport);
    
    x = current_viewport[0];
    y = current_viewport[1];
    w = current_viewport[2];
    h = current_viewport[3];
        
}
    
void GLRenderer::ClearScreen(float *clearColor)
{
	if (nullptr == clearColor)
		glClearColor(0.0, 0.0, 0.0, 0.0); //default black
	else
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);
    glEnable( GL_MULTISAMPLE );
    
    CheckGLError();
}

void GLRenderer::SwapBuffers()
{
	SDL_GL_SwapWindow(mSDLWindow);
}

void GLRenderer::EnableDepthTest( void )
{
    glEnable(GL_DEPTH_TEST);
}
    
void GLRenderer::DisableDepthTest( void )
{
    glDisable(GL_DEPTH_TEST);
}

void GLRenderer::EnableMSAA( void  )
{
    glEnable( GL_MULTISAMPLE );
}
    
void GLRenderer::DisableMSAA( void )
{
    glDisable( GL_MULTISAMPLE );
}
    
void GLRenderer::EnableCulling()
{
	glEnable(GL_CULL_FACE);
}

void GLRenderer::DisableCulling()
{
	glDisable(GL_CULL_FACE);
}

void GLRenderer::CullFrontFace()
{
	glCullFace(GL_FRONT);
}

void GLRenderer::CullBackFace()
{
	glCullFace(GL_BACK);
}

void GLRenderer::SetFrontFaceToCCW()
{
	glFrontFace(GL_CCW);
}

void GLRenderer::SetFrontFaceToCW()
{
	glFrontFace(GL_CW);
}

void GLRenderer::EnableAlphaBlending( void )
{
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
    
void GLRenderer::EnableMaxAlphaBlending( void )
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
        
}
    
void GLRenderer::DisableAlphaBlending( void )
{
    glDisable( GL_BLEND );
}
    
//this method should probably be scrapped as useless
void GLRenderer::SetActiveCamera(Matrix4x4 &viewMatrix, Matrix4x4 &projectionMatrix,
								 float clipNear, float clipFar, float fov)
{
//#define PRINT
#ifdef PRINT
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fov, double(mCurrentViewport.width)/double(mCurrentViewport.height), 
		clipNear, clipFar);

	glGetFloatv(GL_PROJECTION_MATRIX, projMatrix.Data());
	//reset
	//glLoadIdentity();
	cout << "Glu Proj. Matrix: " << endl;
	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			cout << projMatrix(i,j) << " ";
		cout << endl;
	}

	cout << "My Proj. Matrix: " << endl;
	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			cout << projectionMatrix(i,j) << " ";
		cout << endl;
	}

	cout << endl << endl;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);

	glGetFloatv(GL_MODELVIEW_MATRIX, this->viewMatrix.Data());

	cout << "Gl View Matrix: " << endl;
	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			cout << this->viewMatrix(i,j) << " ";
		cout << endl;
	}

	cout << "My View Matrix: " << endl;
	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			cout << viewMatrix(i,j) << " ";
		cout << endl;
	}
	//reset
	//glLoadIdentity();
#endif

}

VertexBuffer* GLRenderer::CreateVertexBuffer( void )
{
    GLVertexBuffer* new_vbo = new GLVertexBuffer();
    return dynamic_cast<VertexBuffer*>(new_vbo);
}
    
IndexBuffer* GLRenderer::CreateIndexBuffer( void )
{
    GLIndexBuffer* new_ibo = new GLIndexBuffer();
    return dynamic_cast<IndexBuffer*>(new_ibo);
}
    
VertexArrayObject* GLRenderer::CreateVertexArrayObject( void )
{
    GLVertexArrayObject* new_vao = new GLVertexArrayObject();
    return dynamic_cast<VertexArrayObject*>(new_vao);
}
    
RenderBuffer* GLRenderer::CreateRenderBuffer( void )
{
    GLRenderBuffer* new_render_buffer = new GLRenderBuffer();
    return dynamic_cast<RenderBuffer*>( new_render_buffer );
}
    
// ----------------
// Textures
// ----------------
    
Texture* GLRenderer::CreateTexture( void )
{
    GLTexture* new_texture = new GLTexture();
    return dynamic_cast<Texture*>(new_texture);
}
    

void GLRenderer::BindTexture( Texture* texture )
{
	glActiveTexture(GL_TEXTURE0 + mTextureStackDepth);

    texture->Bind();

	mTextureStackDepth++;
}

void GLRenderer::UnbindAllTextures()
{
	for (; mTextureStackDepth > 0; )
	{
		glActiveTexture(GL_TEXTURE0 + (--mTextureStackDepth));
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

int GLRenderer::GetActiveTexStackDepth()
{
	return mTextureStackDepth;
}

// ----------------
    
Effect* GLRenderer::CreateShaderProgram(std::string shaderName)
{
	EffectsMap::iterator fx_it = mEffects.find(shaderName);

	if (fx_it != mEffects.end())
	{
		//effect already loaded, no need to do anything
		return (*fx_it).second;
	}
	//else load effect
	Effect *fx = Effect::CreateEffect(shaderName, OPENGL);

	GLSLEffect* glslfx = dynamic_cast<GLSLEffect*>(fx);

	if (!glslfx->LoadEffectFromFile(shaderName))
	{
		return 0;
	}
	else
	{
        CheckGLError();
		mEffects[shaderName] = glslfx;
		// temporary eventually will return Effect*
		return glslfx;
	}
		
}

std::list<ShaderUniform>& GLRenderer::GetShaderUniforms(void)
{	
	assert(mActiveEffect);

	return mActiveEffect->GetShaderUniforms();
}

void GLRenderer::LoadShaderUniforms(void)
{
	if (mActiveEffect == nullptr)
	{
		//log message
		return;
	}

	mActiveEffect->LoadUniforms();
}

void GLRenderer::UseShader(Effect* shader)
{
	if (shader == nullptr)
	{
        mActiveEffect->End();
		mActiveEffect = nullptr;
		return;
	}

	mActiveEffect = dynamic_cast<GLSLEffect*>(shader);
    
    mActiveEffect->Begin();
}
    
}