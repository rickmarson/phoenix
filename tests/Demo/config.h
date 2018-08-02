#ifndef _DEMO_CONFIG_H_
#define _DEMO_CONFIG_H_

#pragma once

// set renderer to test
#define RENDERER NYX::OPENGL 

#if defined(__USE_D3D9__)
//#define RENDERER NYX:D3D9
#elif defined(__USE_D3D11__)
//#define RENDERER NYX:D3D11
#endif

#endif