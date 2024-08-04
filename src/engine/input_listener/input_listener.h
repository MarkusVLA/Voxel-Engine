#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include "../camera/camera.h"

class Window;
class ChunkManager;

class InputListener {
public:
    static void setCamera(Camera* cam);
    static void setWindow(Window* win);
    static void setChunkManager(ChunkManager* cm);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void update();

private:
    static Camera* camera;
    static bool firstMouse;
    static float lastX;
    static float lastY;
    static Window* window;
    static ChunkManager* chunkManager;
    static bool keys[1024];
    static std::chrono::steady_clock::time_point lastEscapePress;
    static const std::chrono::milliseconds escCooldown;
};
