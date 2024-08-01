# Voxel Game Engine

![Voxel Game Engine](img/img.jpeg)

Voxel Engine is a voxel-based game engine and renderer written in C++ using OpenGL and GLFW. 

## Features

- OpenGL-based rendering pipeline
- Voxel rendering and chunk management
- Input handling with GLFW
- Mathematical operations with GLM


### Prerequisites

- CMake 3.2 or higher
- A C++20 compatible compiler
- Git

### Build Instructions

1. **Initialize submodules**:

    ```bash
    git submodule update --init --recursive
    ```

2. **Create a build directory** and navigate into it:

    ```bash
    mkdir build
    cd build
    ```

3. **Configure the project** using CMake:

    ```bash
    cmake ..
    ```

4. **Build the project**:

    ```bash
    cmake --build .
    ```

### Running the Application

After building the project, you can run the executable from the build directory. On windows you may have to move the .exe to the build directory.