#include <glad/glad.h>  
#include "window.h"
#include <iostream>

Window::Window(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, width, height);
    glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, InputListener::keyCallback);
    glfwSetCursorPosCallback(window, InputListener::cursorPositionCallback);
    glfwSetScrollCallback(window, InputListener::scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    cursorEnabled = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

GLFWwindow* Window::getGLFWwindow() {
    return window;
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::toggleCursorMode() {
    cursorEnabled = !cursorEnabled;
    glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool Window::isCursorEnabled() const {
    return cursorEnabled;
}