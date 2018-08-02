/*



*/

#ifndef D3DRENDERER_H
#define D3DRENDERER_H

#pragma once

#include "SDL/SDL_platform.h"

// defines the direct 3d renderer used in the current build
#ifdef __WIN32__
#ifdef __USE_D3D11__
	#include "d3d/d3d11_renderer.h"
	typedef NYX::D3D11Renderer D3DRenderer; 
#endif
#endif

#endif