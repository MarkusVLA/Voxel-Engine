#include "input_listener.h"
#include "../window/window.h"
#include "../../world/chunk/chunk_manager.h"
#include "../../global.h"
#include "GLFW/glfw3.h"
#include <iostream>

Player* InputListener::player = nullptr;
bool InputListener::firstMouse = true;
float InputListener::lastX = WINDOW_WIDTH / 2.0f;
float InputListener::lastY = WINDOW_HEIGHT / 2.0f;
Window* InputListener::window = nullptr;
ChunkManager* InputListener::chunkManager = nullptr;
bool InputListener::keys[1024] = { false };
std::chrono::steady_clock::time_point InputListener::lastEscapePress = std::chrono::steady_clock::now();
const std::chrono::milliseconds InputListener::escCooldown = std::chrono::milliseconds(200); // 200ms cooldown

void InputListener::setPlayer(Player* p) { player = p; }
void InputListener::setWindow(Window* win) { window = win; }
void InputListener::setChunkManager(ChunkManager* cm) { chunkManager = cm; }

void InputListener::keyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEscapePress) >= escCooldown) {
            window->toggleCursorMode();
            lastEscapePress = now;
        }
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void InputListener::update() {
    if (!window->isCursorEnabled() && player) {
        float deltaTime = window->getDeltaTime();
        if (keys[GLFW_KEY_W]) player->move(FORWARD, deltaTime);
        if (keys[GLFW_KEY_S]) player->move(BACKWARD, deltaTime);
        if (keys[GLFW_KEY_A]) player->move(LEFT, deltaTime);
        if (keys[GLFW_KEY_D]) player->move(RIGHT, deltaTime);
        if (keys[GLFW_KEY_SPACE]) player->move(UP, deltaTime);
        if (keys[GLFW_KEY_LEFT_SHIFT]) player->move(DOWN, deltaTime);
    }
}

void InputListener::cursorPositionCallback(GLFWwindow* glfwWindow, double xpos, double ypos) {
    if (!window->isCursorEnabled()) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        player->getCamera()->processMouseMovement(xoffset, yoffset);
    }
}

void InputListener::scrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    if (!window->isCursorEnabled()) {
        player->getCamera()->processMouseScroll(yoffset);
    }
}

void InputListener::mouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
    if (action == GLFW_PRESS && !window->isCursorEnabled() && player && chunkManager) {
        glm::vec3 cameraPos = player->getCamera()->getPosition();
        glm::vec3 cameraFront = player->getCamera()->getFront();
        float maxInteractionDistance = 5.0f;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            // Add left mouse button action here
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            // Add right mouse button action here
        }
    }
}
