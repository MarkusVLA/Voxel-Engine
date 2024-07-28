#include "input_listener.h"
#include <iostream>

Camera* InputListener::camera = nullptr;
bool InputListener::firstMouse = true;
float InputListener::lastX = 800.0f / 2.0f; // Screen width / 2
float InputListener::lastY = 600.0f / 2.0f; // Screen height / 2

void InputListener::setCamera(Camera* cam) {
    camera = cam;
}

void InputListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        if (camera) {
            camera->processKeyboard(key, 0.05f); // Pass delta time as needed
        }
    }
}

void InputListener::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (camera) {
        camera->processMouseMovement(xoffset, yoffset);
    }
}

void InputListener::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (camera) {
        camera->processMouseScroll(yoffset);
    }
}
