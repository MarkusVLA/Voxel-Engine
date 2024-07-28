cmake_minimum_required(VERSION 3.2)
project(OpenGL)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)

add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PRIVATE libs/glad/include)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(libs/glad/include)
include_directories(libs/glfw/include)
include_directories(libs/glm)
include_directories(libs/stb)

add_executable(app 
    src/main.cpp
    src/engine/renderer/renderer.cpp
    src/engine/renderer/shader.cpp
    src/engine/renderer/texture_loader.cpp
    src/engine/window/window.cpp
    src/engine/input_listener/input_listener.cpp
    src/engine/camera/camera.cpp
    src/world/voxel/voxel.cpp
    src/world/chunk.cpp

)

target_link_libraries(app ${OPENGL_gl_LIBRARY} glm glfw glad)
