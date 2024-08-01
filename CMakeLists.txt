cmake_minimum_required(VERSION 3.2)
project(OpenGL)
set(CMAKE_CXX_STANDARD 20)

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
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(imgui PUBLIC 
    ${OPENGL_gl_LIBRARY}
    glfw
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
    src/world/skybox.cpp
    src/world/chunk/chunk_manager.cpp
    src/utils/perlin.cpp
)

target_link_libraries(app ${OPENGL_gl_LIBRARY} glm glfw glad freetype imgui)