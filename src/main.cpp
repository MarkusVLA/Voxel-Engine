#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/renderer/renderer.h"
#include "engine/window/window.h"
#include "engine/input_listener/input_listener.h"
#include "engine/camera/camera.h"
#include "world/skybox.h"
#include "world/chunk/chunk_manager.h"
#include "engine/window/GUI/gui.h"
#include <iostream>

int main() {
    Window window(1000, 600, "OpenGL Window");
    Camera camera(glm::vec3(0.0f, 128.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    InputListener::setCamera(&camera);
    InputListener::setWindow(&window);
    SkyBox skyBox;

    int chunkWidth = 16;
    int chunkHeight = 256;
    int chunkDepth = 16;
    int viewDistance = 16;
    ChunkManager chunkManager(chunkWidth, chunkHeight, chunkDepth, viewDistance);
    InputListener::setChunkManager(&chunkManager);

    // Set input callbacks
    glfwSetKeyCallback(window.getGLFWwindow(), InputListener::keyCallback);
    glfwSetCursorPosCallback(window.getGLFWwindow(), InputListener::cursorPositionCallback);
    glfwSetScrollCallback(window.getGLFWwindow(), InputListener::scrollCallback);
    glfwSetMouseButtonCallback(window.getGLFWwindow(), InputListener::mouseButtonCallback);

    chunkManager.updatePlayerPosition(camera.getPosition());

    Renderer renderer;
    renderer.setCamera(&camera);
    renderer.initShaders();
    renderer.loadTexture("../assets/textures/atlas.png");
    renderer.setSkyboxData(skyBox.GetVertices());

    GUI gui(window.getGLFWwindow());
    glfwSetWindowUserPointer(window.getGLFWwindow(), &gui);

    double lastFrame = glfwGetTime();
    glm::vec3 lastUpdatePosition = camera.getPosition();
    int frameCount = 0;
    float fps = 0.0f;
    double lastFPSPrintTime = glfwGetTime();

    while (!window.shouldClose()) {
        double currentFrame = glfwGetTime();
        double deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        window.pollEvents();

        glm::vec3 currentCameraPos = camera.getPosition();
        if (glm::distance(currentCameraPos, lastUpdatePosition) > chunkWidth / 2.0f) {
            chunkManager.updatePlayerPosition(currentCameraPos);
            lastUpdatePosition = currentCameraPos;
        }

        std::tuple<glm::ivec2, std::vector<float>, std::vector<unsigned int>, std::vector<float>, std::vector<unsigned int>> item;
        while (chunkManager.getRenderQueue().tryPop(item)) {
            const auto& [chunkPos, solidVertices, solidIndices, waterVertices, waterIndices] = item;
            if ((!solidVertices.empty() && !solidIndices.empty()) || (!waterVertices.empty() && !waterIndices.empty())) {
                renderer.addChunk(chunkPos, solidVertices, solidIndices, waterVertices, waterIndices);
            } else {
                renderer.removeChunk(chunkPos);
            }
        }

        renderer.processChunkUpdates();
        renderer.setLightDir(gui.getLightDirection());

        frameCount++;
        if (currentFrame - lastFPSPrintTime >= 1.0) {
            fps = frameCount / (currentFrame - lastFPSPrintTime);
            frameCount = 0;
            lastFPSPrintTime = currentFrame;
        }

        gui.newFrame();
        gui.displayInfo(fps, camera.getPosition(), viewDistance, chunkManager.getLoadedChunksCount());
        renderer.draw();
        gui.render();
        gui.drawCrosshair();
        window.swapBuffers();
    }

    return 0;
}