cmake_minimum_required(VERSION 3.2)
project(OpenGL)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG")
endif()

find_package(OpenGL REQUIRED)
include_directories(libs)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/freetype)

add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PRIVATE libs/glad/include)

set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    libs/imgui
    libs/imgui/backends
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(imgui PUBLIC
    ${OPENGL_gl_LIBRARY}
)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(libs/glad/include)
include_directories(libs/glfw/include)
include_directories(libs/glm)
include_directories(libs/stb)
include_directories(libs/freetype/include)
include_directories(libs/imgui)
include_directories(libs/imgui/backends)

add_executable(app
    src/main.cpp
    src/engine/renderer/renderer.cpp
    src/engine/renderer/shader.cpp
    src/engine/renderer/texture_loader.cpp
    src/engine/window/window.cpp
    src/engine/window/GUI/gui.cpp
    src/engine/input_listener/input_listener.cpp
    src/engine/camera/camera.cpp
    src/world/voxel/voxel.cpp
    src/world/chunk/chunk.cpp
    src/world/chunk/terrain_generator/terrain_generator.cpp
    src/world/skybox.cpp
    src/world/chunk/chunk_manager.cpp
    src/utils/perlin.cpp
)

target_link_libraries(app ${OPENGL_gl_LIBRARY} glm glfw glad freetype imgui)

set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/game/bin
)

add_custom_command(TARGET app PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/game/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/game/shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/game/assets
)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/game/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/game/assets
)

add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_BINARY_DIR}/compile_commands.json
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
)

add_dependencies(app copy_compile_commands)
