#include "input_listener.h"
#include "../window/window.h"
#include <iostream>

Camera* InputListener::camera = nullptr;
bool InputListener::firstMouse = true;
float InputListener::lastX = 800.0f / 2.0f;
float InputListener::lastY = 600.0f / 2.0f;
Window* InputListener::window = nullptr;
std::chrono::steady_clock::time_point InputListener::lastEscapePress = std::chrono::steady_clock::now();
const std::chrono::milliseconds InputListener::escCooldown = std::chrono::milliseconds(200); // 200ms cooldown

void InputListener::setCamera(Camera* cam) {
    camera = cam;
}

void InputListener::setWindow(Window* win) {
    window = win;
}

void InputListener::keyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEscapePress) >= escCooldown) {
            window->toggleCursorMode();
            lastEscapePress = now;
        }
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (!window->isCursorEnabled() && camera) {
            switch (key) {
                case GLFW_KEY_W:
                    camera->processKeyboard(FORWARD, 0.01f);
                    break;
                case GLFW_KEY_S:
                    camera->processKeyboard(BACKWARD, 0.01f);
                    break;
                case GLFW_KEY_A:
                    camera->processKeyboard(LEFT, 0.01f);
                    break;
                case GLFW_KEY_D:
                    camera->processKeyboard(RIGHT, 0.01f);
                    break;
                default:
                    break;
            }
        }
    }
}

void InputListener::cursorPositionCallback(GLFWwindow* glfwWindow, double xpos, double ypos) {
    if (!window->isCursorEnabled() && camera) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        camera->processMouseMovement(xoffset, yoffset);
    }
}

void InputListener::scrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    if (!window->isCursorEnabled() && camera) {
        camera->processMouseScroll(yoffset);
    }
}