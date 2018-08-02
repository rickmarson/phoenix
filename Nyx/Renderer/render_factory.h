/*

Render Factory

*/
#ifndef RENDERFACTORY_H
#define RENDERFACTORY_H

#include "ogl/gl_renderer.h"
#if !PLATFORM_MAC && !DISABLE_D3D
#include "d3d/d3d_renderer.h"
#endif

#if PLATFORM_MAC
#include <SDL2/SDL_platform.h>
#else
#include "SDL/SDL_platform.h"
#endif


namespace NYX {

	NYX_EXPORT IRenderer* CreateRenderer(SDL_Window *win, SDL_SysWMinfo winInfo, eRenderer backend = OPENGL) 
	{
		IRenderer *ren = NULL;

#if defined(__WIN32__) && !DISABLE_D3D
		switch (backend)
		{
		case OPENGL: 
		{
			GLRenderer *gl = new GLRenderer(win, winInfo);
			ren = dynamic_cast<IRenderer*>(gl);
			break;
		}
		case D3D:
		{
			D3DRenderer* d3d = new D3DRenderer(win, winInfo);
			ren = dynamic_cast<IRenderer*>(d3d);
			break;
		}
		default:
			break;
		}
#else
		GLRenderer *gl = new GLRenderer(win, winInfo);
		ren = dynamic_cast<IRenderer*>(gl);
#endif

		return ren;

	}

}

#endif
