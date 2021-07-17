set(PROJECT_NAME Phoenix)

################################################################################
# Source groups
################################################################################
set(Cache
    "../../cache/iresources.h"
    "../../cache/resource.cpp"
    "../../cache/resource.h"
    "../../cache/resource_cache.cpp"
    "../../cache/resource_cache.h"
)
source_group("Cache" FILES ${Cache})

set(Events
    "../../Events/event_manager.cpp"
    "../../Events/event_manager.h"
    "../../Events/events.cpp"
    "../../Events/events.h"
    "../../Events/ievent.h"
)
source_group("Events" FILES ${Events})

set(Generic
    "../../application.cpp"
    "../../application.h"
    "../../config.h"
    "../../material.cpp"
    "../../material.h"
    "../../mesh.cpp"
    "../../mesh.h"
    "../../model.cpp"
    "../../model.h"
    "../../particlefx.cpp"
    "../../particlefx.h"
    "../../scene.cpp"
    "../../scene.h"
    "../../window.cpp"
    "../../window.h"
    "../../window_state.cpp"
    "../../window_state.h"
)
source_group("Generic" FILES ${Generic})

set(Math
    "../../Math/constants.h"
    "../../Math/matrix3x3.cpp"
    "../../Math/matrix3x3.h"
    "../../Math/matrix4x4.cpp"
    "../../Math/matrix4x4.h"
    "../../Math/quaternion.cpp"
    "../../Math/quaternion.h"
    "../../Math/vector2.cpp"
    "../../Math/vector2.h"
    "../../Math/vector3.cpp"
    "../../Math/vector3.h"
    "../../Math/vector4.cpp"
    "../../Math/vector4.h"
)
source_group("Math" FILES ${Math})

set(Physics
    "../../Physics/body.cpp"
    "../../Physics/body.h"
    "../../Physics/iphysics_engine.h"
    "../../Physics/physics_engine.cpp"
    "../../Physics/physics_engine.h"
    "../../Physics/rigid_body.cpp"
    "../../Physics/rigid_body.h"
    "../../Physics/soft_body.cpp"
    "../../Physics/soft_body.h"
)
source_group("Physics" FILES ${Physics})

set(Renderer
    "../../Renderer/effect.cpp"
    "../../Renderer/effect.h"
    "../../Renderer/fx_manager.cpp"
    "../../Renderer/fx_manager.h"
    "../../Renderer/index_buffer.h"
    "../../Renderer/irenderer.h"
    "../../Renderer/render_buffer.h"
    "../../Renderer/render_factory.h"
    "../../Renderer/shader_uniform.cpp"
    "../../Renderer/shader_uniform.h"
    "../../Renderer/shader_utils.cpp"
    "../../Renderer/shader_utils.h"
    "../../Renderer/texture.h"
    "../../Renderer/vertex_array_object.h"
    "../../Renderer/vertex_buffer.h"
)
source_group("Renderer" FILES ${Renderer})

set(Renderer__D3D
#    "../../Renderer/d3d/d3d11_effect.cpp"
#    "../../Renderer/d3d/d3d11_effect.h"
#    "../../Renderer/d3d/d3d11_index_buffer.cpp"
#    "../../Renderer/d3d/d3d11_index_buffer.h"
#    "../../Renderer/d3d/d3d11_render_buffer.cpp"
#    "../../Renderer/d3d/d3d11_render_buffer.h"
#    "../../Renderer/d3d/d3d11_renderer.cpp"
#    "../../Renderer/d3d/d3d11_renderer.h"
#    "../../Renderer/d3d/d3d11_shader_utils.cpp"
#    "../../Renderer/d3d/d3d11_shader_utils.h"
#    "../../Renderer/d3d/d3d11_texture.cpp"
#    "../../Renderer/d3d/d3d11_texture.h"
#    "../../Renderer/d3d/d3d11_vertex_array_object.cpp"
#    "../../Renderer/d3d/d3d11_vertex_array_object.h"
#    "../../Renderer/d3d/d3d11_vertex_buffer.cpp"
#    "../../Renderer/d3d/d3d11_vertex_buffer.h"
#    "../../Renderer/d3d/d3d_renderer.h"
)
source_group("Renderer\\D3D" FILES ${Renderer__D3D})

set(Renderer__GL
    "../../Renderer/ogl/gl_index_buffer.cpp"
    "../../Renderer/ogl/gl_index_buffer.h"
    "../../Renderer/ogl/gl_render_buffer.cpp"
    "../../Renderer/ogl/gl_render_buffer.h"
    "../../Renderer/ogl/gl_renderer.cpp"
    "../../Renderer/ogl/gl_renderer.h"
    "../../Renderer/ogl/gl_texture.cpp"
    "../../Renderer/ogl/gl_texture.h"
    "../../Renderer/ogl/gl_utils.cpp"
    "../../Renderer/ogl/gl_utils.h"
    "../../Renderer/ogl/gl_vertex_array_object.cpp"
    "../../Renderer/ogl/gl_vertex_array_object.h"
    "../../Renderer/ogl/gl_vertex_buffer.cpp"
    "../../Renderer/ogl/gl_vertex_buffer.h"
    "../../Renderer/ogl/glsl_effect.cpp"
    "../../Renderer/ogl/glsl_effect.h"
    "../../Renderer/ogl/glsl_shader_utils.cpp"
    "../../Renderer/ogl/glsl_shader_utils.h"
)
source_group("Renderer\\GL" FILES ${Renderer__GL})

set(Resource_Files
    "../../../Resources/create_dest_tree.bat"
)
source_group("Resource Files" FILES ${Resource_Files})

set(Resource_Files__FX_Kernels
    "../../../Resources/FXKernels/particle_fountain.cl"
)
source_group("Resource Files\\FX Kernels" FILES ${Resource_Files__FX_Kernels})

set(Resource_Files__Fonts
    "../../../Resources/fonts/Apache License.txt"
#    "../../../Resources/fonts/Capture_it.ttf"
#    "../../../Resources/fonts/Cpr.Sparhelt font License.txt"
#    "../../../Resources/fonts/OpenSans-Regular.ttf"
)
source_group("Resource Files\\Fonts" FILES ${Resource_Files__Fonts})

set(Resource_Files__Models
    "../../../Resources/Models/Cube.mtl"
    "../../../Resources/Models/Cube.obj"
    "../../../Resources/Models/Marble.mtl"
    "../../../Resources/Models/Marble.obj"
    "../../../Resources/Models/Mars.mtl"
    "../../../Resources/Models/Mars.obj"
    "../../../Resources/Models/Particle1.mtl"
)
source_group("Resource Files\\Models" FILES ${Resource_Files__Models})

set(Resource_Files__Scripts
    "../../../Resources/Scripts/camera_anim.lua"
    "../../../Resources/Scripts/particle_fountain.lua"
)
source_group("Resource Files\\Scripts" FILES ${Resource_Files__Scripts})

set(Resource_Files__Shaders
#    "../../../Resources/Shaders/Basic.shader"
#    "../../../Resources/Shaders/BasicParticle.shader"
#    "../../../Resources/Shaders/CubeMapReflection.shader"
#    "../../../Resources/Shaders/Flat.shader"
#    "../../../Resources/Shaders/SkyBox.shader"
    "../../../Resources/Shaders/UIShader.shader"
)
source_group("Resource Files\\Shaders" FILES ${Resource_Files__Shaders})

set(Resource_Files__Textures
    "../../../Resources/Textures/marble_texture4662.jpg"
    "../../../Resources/Textures/MarsElevation_2500x1250.jpg"
    "../../../Resources/Textures/MarsMap_2500x1250.jpg"
    "../../../Resources/Textures/mountain_negx.jpg"
    "../../../Resources/Textures/mountain_negy.jpg"
    "../../../Resources/Textures/mountain_negz.jpg"
    "../../../Resources/Textures/mountain_posx.jpg"
    "../../../Resources/Textures/mountain_posy.jpg"
    "../../../Resources/Textures/mountain_posz.jpg"
)
source_group("Resource Files\\Textures" FILES ${Resource_Files__Textures})

set(Scene
    "../../scene/camera_fp.cpp"
    "../../scene/camera_fp.h"
    "../../scene/camera_node.cpp"
    "../../scene/camera_node.h"
    "../../scene/camera_tp.cpp"
    "../../scene/camera_tp.h"
    "../../scene/frustum.cpp"
    "../../scene/frustum.h"
    "../../scene/light_node.cpp"
    "../../scene/light_node.h"
    "../../scene/mesh_node.cpp"
    "../../scene/mesh_node.h"
    "../../scene/model_node.cpp"
    "../../scene/model_node.h"
    "../../scene/particle_node.cpp"
    "../../scene/particle_node.h"
    "../../scene/root_node.cpp"
    "../../scene/root_node.h"
    "../../scene/scene_node.cpp"
    "../../scene/scene_node.h"
    "../../scene/skybox.cpp"
    "../../scene/skybox.h"
)
source_group("Scene" FILES ${Scene})

set(Scripts
    "../../Script/script_manager.cpp"
    "../../Script/script_manager.h"
)
source_group("Scripts" FILES ${Scripts})

set(Thirdparty__Json
    "../../../../thirdparty/Json/json.h"
    "../../../../thirdparty/Json/json.hpp"
)
source_group("Thirdparty\\Json" FILES ${Thirdparty__Json})

set(Thirdparty__LuaPlus
    "../../../../thirdparty/LuaPlus/License.txt"
    "../../../../thirdparty/LuaPlus/LuaAutoBlock.h"
    "../../../../thirdparty/LuaPlus/LuaCall.h"
    "../../../../thirdparty/LuaPlus/LuaCall.inl"
    "../../../../thirdparty/LuaPlus/LuaFunction.h"
    "../../../../thirdparty/LuaPlus/LuaHelper.h"
    "../../../../thirdparty/LuaPlus/LuaHelper_Object.h"
    "../../../../thirdparty/LuaPlus/LuaHelper_StackObject.h"
    "../../../../thirdparty/LuaPlus/LuaObject.h"
    "../../../../thirdparty/LuaPlus/LuaObject.inl"
    "../../../../thirdparty/LuaPlus/LuaPlus.cpp"
    "../../../../thirdparty/LuaPlus/LuaPlus.h"
    "../../../../thirdparty/LuaPlus/LuaPlusAddons.c"
    "../../../../thirdparty/LuaPlus/LuaPlusCD.h"
    "../../../../thirdparty/LuaPlus/LuaPlusConfig.h"
    "../../../../thirdparty/LuaPlus/LuaPlusInternal.h"
    "../../../../thirdparty/LuaPlus/LuaStackObject.h"
    "../../../../thirdparty/LuaPlus/LuaStackObject.inl"
    "../../../../thirdparty/LuaPlus/LuaStackTableIterator.h"
    "../../../../thirdparty/LuaPlus/LuaStackTableIterator.inl"
    "../../../../thirdparty/LuaPlus/LuaState.h"
    "../../../../thirdparty/LuaPlus/LuaState.inl"
    "../../../../thirdparty/LuaPlus/LuaState_DumpObject.cpp"
    "../../../../thirdparty/LuaPlus/LuaStateCD.h"
    "../../../../thirdparty/LuaPlus/LuaStateOutFile.h"
    "../../../../thirdparty/LuaPlus/LuaStateOutString.h"
    "../../../../thirdparty/LuaPlus/LuaTableIterator.h"
)
source_group("Thirdparty\\LuaPlus" FILES ${Thirdparty__LuaPlus})

set(Thirdparty__LuaPlus__Lua
    "../../../../thirdparty/LuaPlus/src/lapi.c"
    "../../../../thirdparty/LuaPlus/src/lapi.h"
    "../../../../thirdparty/LuaPlus/src/lauxlib.c"
    "../../../../thirdparty/LuaPlus/src/lauxlib.h"
    "../../../../thirdparty/LuaPlus/src/lbaselib.c"
    "../../../../thirdparty/LuaPlus/src/lbitlib.c"
    "../../../../thirdparty/LuaPlus/src/lcode.c"
    "../../../../thirdparty/LuaPlus/src/lcode.h"
    "../../../../thirdparty/LuaPlus/src/lcorolib.c"
    "../../../../thirdparty/LuaPlus/src/lctype.c"
    "../../../../thirdparty/LuaPlus/src/lctype.h"
    "../../../../thirdparty/LuaPlus/src/ldblib.c"
    "../../../../thirdparty/LuaPlus/src/ldebug.c"
    "../../../../thirdparty/LuaPlus/src/ldebug.h"
    "../../../../thirdparty/LuaPlus/src/ldo.c"
    "../../../../thirdparty/LuaPlus/src/ldo.h"
    "../../../../thirdparty/LuaPlus/src/ldump.c"
    "../../../../thirdparty/LuaPlus/src/lfunc.c"
    "../../../../thirdparty/LuaPlus/src/lfunc.h"
    "../../../../thirdparty/LuaPlus/src/lgc.c"
    "../../../../thirdparty/LuaPlus/src/lgc.h"
    "../../../../thirdparty/LuaPlus/src/linit.c"
    "../../../../thirdparty/LuaPlus/src/liolib.c"
    "../../../../thirdparty/LuaPlus/src/llex.c"
    "../../../../thirdparty/LuaPlus/src/llex.h"
    "../../../../thirdparty/LuaPlus/src/llimits.h"
    "../../../../thirdparty/LuaPlus/src/lmathlib.c"
    "../../../../thirdparty/LuaPlus/src/lmem.c"
    "../../../../thirdparty/LuaPlus/src/lmem.h"
    "../../../../thirdparty/LuaPlus/src/loadlib.c"
    "../../../../thirdparty/LuaPlus/src/lobject.c"
    "../../../../thirdparty/LuaPlus/src/lobject.h"
    "../../../../thirdparty/LuaPlus/src/lopcodes.c"
    "../../../../thirdparty/LuaPlus/src/lopcodes.h"
    "../../../../thirdparty/LuaPlus/src/loslib.c"
    "../../../../thirdparty/LuaPlus/src/lparser.c"
    "../../../../thirdparty/LuaPlus/src/lparser.h"
    "../../../../thirdparty/LuaPlus/src/lprefix.h"
    "../../../../thirdparty/LuaPlus/src/lstate.c"
    "../../../../thirdparty/LuaPlus/src/lstate.h"
    "../../../../thirdparty/LuaPlus/src/lstring.c"
    "../../../../thirdparty/LuaPlus/src/lstring.h"
    "../../../../thirdparty/LuaPlus/src/lstrlib.c"
    "../../../../thirdparty/LuaPlus/src/ltable.c"
    "../../../../thirdparty/LuaPlus/src/ltable.h"
    "../../../../thirdparty/LuaPlus/src/ltablib.c"
    "../../../../thirdparty/LuaPlus/src/ltm.c"
    "../../../../thirdparty/LuaPlus/src/ltm.h"
    "../../../../thirdparty/LuaPlus/src/lua.c"
    "../../../../thirdparty/LuaPlus/src/lua.h"
    "../../../../thirdparty/LuaPlus/src/lua.hpp"
    "../../../../thirdparty/LuaPlus/src/luac.c"
    "../../../../thirdparty/LuaPlus/src/luaconf.h"
    "../../../../thirdparty/LuaPlus/src/lualib.h"
    "../../../../thirdparty/LuaPlus/src/lundump.c"
    "../../../../thirdparty/LuaPlus/src/lundump.h"
    "../../../../thirdparty/LuaPlus/src/lutf8lib.c"
    "../../../../thirdparty/LuaPlus/src/lvm.c"
    "../../../../thirdparty/LuaPlus/src/lvm.h"
    "../../../../thirdparty/LuaPlus/src/lzio.c"
    "../../../../thirdparty/LuaPlus/src/lzio.h"
)
source_group("Thirdparty\\LuaPlus\\Lua" FILES ${Thirdparty__LuaPlus__Lua})

set(Thirdparty__stb
    "../../../../thirdparty/stb/README.md"
    "../../../../thirdparty/stb/stb.h"
    "../../../../thirdparty/stb/stb_c_lexer.h"
    "../../../../thirdparty/stb/stb_divide.h"
    "../../../../thirdparty/stb/stb_dxt.h"
    "../../../../thirdparty/stb/stb_easy_font.h"
    "../../../../thirdparty/stb/stb_herringbone_wang_tile.h"
    "../../../../thirdparty/stb/stb_image.c"
    "../../../../thirdparty/stb/stb_image.h"
    "../../../../thirdparty/stb/stb_image_resize.h"
    "../../../../thirdparty/stb/stb_image_write.h"
    "../../../../thirdparty/stb/stb_leakcheck.h"
    "../../../../thirdparty/stb/stb_perlin.h"
    "../../../../thirdparty/stb/stb_rect_pack.h"
    "../../../../thirdparty/stb/stb_textedit.h"
    "../../../../thirdparty/stb/stb_tilemap_editor.h"
    "../../../../thirdparty/stb/stb_truetype.h"
    "../../../../thirdparty/stb/stb_vorbis.c"
    "../../../../thirdparty/stb/stretchy_buffer.h"
)
source_group("Thirdparty\\stb" FILES ${Thirdparty__stb})

set(UI
    "../../UI/label.cpp"
    "../../UI/label.h"
    "../../UI/ui_events.h"
    "../../UI/ui_manager.cpp"
    "../../UI/ui_manager.h"
    "../../UI/widget.cpp"
    "../../UI/widget.h"
)
source_group("UI" FILES ${UI})

set(Utils
    "../../Utils/file_manager.cpp"
    "../../Utils/file_manager.h"
    "../../Utils/hash.cpp"
    "../../Utils/hash.h"
    "../../Utils/log_manager.cpp"
    "../../Utils/log_manager.h"
    "../../Utils/singleton.h"
)
source_group("Utils" FILES ${Utils})

set(ALL_FILES
    ${Cache}
    ${Events}
    ${Generic}
    ${Math}
    ${Physics}
    ${Renderer}
    ${Renderer__D3D}
    ${Renderer__GL}
    ${Resource_Files}
    ${Resource_Files__FX_Kernels}
    ${Resource_Files__Fonts}
    ${Resource_Files__Models}
    ${Resource_Files__Scripts}
    ${Resource_Files__Shaders}
    ${Resource_Files__Textures}
    ${Scene}
    ${Scripts}
    ${Thirdparty__Json}
    ${Thirdparty__LuaPlus}
    ${Thirdparty__LuaPlus__Lua}
    ${Thirdparty__stb}
    ${UI}
    ${Utils}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Nyx)

################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "${PROJECT_NAME}_debug"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/../../lib/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/../../lib/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cache;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Events;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Math;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Utils;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Scripts;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../scene;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Physics;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../UI;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/SDL;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/SDL/SDL_ttf;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/Bullet;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/LuaPlus/src"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "DEBUG"
    ">"
    "_MBCS;"
    "_CRT_SECURE_NO_WARNINGS;"
    "_SCL_SECURE_NO_WARNINGS;"
    "__STATIC_BUILD__;"
    "__USE_D3D11__"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /Gy
        >
        /sdl;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        /wd4305;
        /wd4244;
        /wd4251;
        /wd4316;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
    )
endif()

################################################################################
# Pre build events
################################################################################
add_custom_command_if(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMANDS
    COMMAND   $<CONFIG:Debug> $<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\..\\..\\Resources\\create_dest_tree.bat $<SHELL_PATH:${CMAKE_SOURCE_DIR}/>
    COMMAND $<CONFIG:Release> $<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\..\\..\\Resources\\create_dest_tree.bat $(SolutionDir)
)

