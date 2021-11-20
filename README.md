**This project is no longer being developed**

A sandbox for experimenting with various game engine components and ideas. 

## Highlights

- Shader abstraction
- Graphics API abstraction
- OpenGL / OpenCL interop. Particle simulation using OpenCL, rendered in OpenGL
- Lua script integration
- Bullet physics integration
- Simple scene graph

## Demo

The Demo project contains a set of very simple scenes to test out basic functionality. 

## List of third party library

- [bullet physics](https://github.com/bulletphysics/bullet3)
- [Glew](http://glew.sourceforge.net/)
- [json](https://github.com/nlohmann/json)
- [LuaPlus](https://github.com/jjensen/luaplus51-all)
- [OpenCL](https://www.khronos.org/opencl/)
- [physfs](https://github.com/icculus/physfs)
- [SDL2](https://www.libsdl.org/)
- [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [stb](https://github.com/nothings/stb)

## Compatibility 

Last tested with Windows 11 + Visual Studio 2022
Originally, OSX was supported but XCode projects have since been removed. 
Mac-specific code is still in the repo, so it might still work in OSX though new projects would need to be created. 

The NVidia CUDA toolkit, or another source of the OpenCL development libraries is required. 
