set(PROJECT_NAME Demo)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../data/config.json"
    "../../data/particle test.json"
    "../../data/physics test.json"
    "../../data/space scene.json"
)
source_group("" FILES ${no_group_source_files})

set(Generic
    "../../config.h"
    "../../demo.cpp"
    "../../particle_test.cpp"
    "../../particle_test.h"
    "../../physics_test.cpp"
    "../../physics_test.h"
    "../../space_scene.cpp"
    "../../space_scene.h"
)
source_group("Generic" FILES ${Generic})

set(dlls
#    "../../../../../thirdparty/win/FreeImage.dll"
#    "../../../../../thirdparty/win/glew32.dll"
    "../../../../../thirdparty/win/libfreetype-6.dll"
#    "../../../../../thirdparty/win/physfs.dll"
    "../../../../../thirdparty/win/SDL2.dll"
    "../../../../../thirdparty/win/SDL2_ttf.dll"
    "../../../../../thirdparty/win/zlib1.dll"
)
source_group("dlls" FILES ${dlls})

set(ALL_FILES
    ${no_group_source_files}
    ${Generic}
    ${dlls}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Demo)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "${PROJECT_NAME}d"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}//../../bin/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}//../../bin/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Game;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/cache;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/Events;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/Math;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/Utils;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/Scripts;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/scene;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/Renderer;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/Physics;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Nyx/UI;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../thirdparty;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../thirdparty/Bullet;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../thirdparty/SDL"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "DEBUG;"
        "__DEBUG__"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "WIN32;"
    "_CONSOLE;"
    "_LIB;"
    "_CRT_SECURE_NO_WARNINGS;"
    "_SCL_SECURE_NO_WARNINGS;"
    "__STATIC_BUILD__;"
    "UNICODE;"
    "_UNICODE"
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
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        /wd4305;
        /wd4244;
        /wd4251;
        /wd4316;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL;
            /ignore:4099
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /DEBUG;
        /SUBSYSTEM:CONSOLE;
        /NODEFAULTLIB:msvcrt.lib
    )
endif()

################################################################################
# Custom build events
################################################################################
add_custom_command_if(
    TARGET ${PROJECT_NAME}
    COMMANDS
    COMMAND   $<CONFIG:Debug> %dest_dir%\\Resources\\Fonts MKDIR %dest_dir%\\Resources\\Fonts
    COMMAND $<CONFIG:Release> %dest_dir%\\Resources\\Fonts MKDIR %dest_dir%\\Resources\\Fonts
    COMMENT "Copying Data"
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    Phoenix
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    Phoenix
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "opengl32;"
    "glu32;"
    "d3d11;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\glew32;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\physfs;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\SDL2;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\SDL2main;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\SDL2_ttf;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\BulletCollision_vs2010_debug;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\BulletDynamics_vs2010_debug;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\BulletSoftBody_vs2010_debug;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\LinearMath_vs2010_debug;"
    "$(SolutionDir)..\\..\\..\\thirdparty\\Win\\OpenCL"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

