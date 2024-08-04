// game.h
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/renderer/renderer.h"
#include "engine/window/window.h"
#include "engine/input_listener/input_listener.h"
#include "engine/camera/camera.h"
#include "glm/fwd.hpp"
#include "world/skybox.h"
#include "world/chunk/chunk_manager.h"
#include "engine/window/GUI/gui.h"
#include <iostream>
#include "global.h"

class Game {
public:
    Game();
    void run();

private:
    void initialize();

    Window window;
    Camera camera;
    SkyBox skyBox;
    ChunkManager chunkManager;
    Renderer renderer;
    GUI gui;

    glm::vec3 lastUpdatePosition;
    double lastFrame;
    int frameCount;
    float fps;
    double lastFPSPrintTime;
};
