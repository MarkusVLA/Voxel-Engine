#include "input_listener.h"
#include <iostream>

Camera* InputListener::camera = nullptr;
bool InputListener::firstMouse = true;
float InputListener::lastX = 800.0f / 2.0f; 
float InputListener::lastY = 600.0f / 2.0f; 

bool InputListener::isCameraActive = true;


void InputListener::setCamera(Camera* cam) {
    camera = cam;
}

void InputListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            isCameraActive = !isCameraActive;
            glfwSetInputMode(window, GLFW_CURSOR, isCameraActive ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            return;
        }
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (isCameraActive && camera) {
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

void InputListener::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (isCameraActive && camera) {
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


void InputListener::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (isCameraActive && camera) {
        camera->processMouseScroll(yoffset);
    }
}