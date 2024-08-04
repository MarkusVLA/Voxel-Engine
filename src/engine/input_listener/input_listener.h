#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include "../../world/player/player.h"


class Window;
class ChunkManager;

class InputListener {
public:
    static void setWindow(Window* win);
    static void setChunkManager(ChunkManager* cm);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void update();
    static void setPlayer(Player* player);

private:

    static Player* player;
    static bool firstMouse;
    static float lastX;
    static float lastY;
    static Window* window;
    static ChunkManager* chunkManager;
    static bool keys[1024];
    static std::chrono::steady_clock::time_point lastEscapePress;
    static const std::chrono::milliseconds escCooldown;
};
