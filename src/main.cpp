#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/renderer/renderer.h"
#include "engine/window/window.h"
#include "engine/input_listener/input_listener.h"
#include "engine/camera/camera.h"
#include "world/skybox.h"
#include "world/chunk/chunk_manager.h"
#include <iostream>

int main() {
    try {
        Window window(1920, 1200, "OpenGL Window");

        Camera camera(glm::vec3(0.0f, 128.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
        InputListener::setCamera(&camera);

        SkyBox skyBox;

        int chunkWidth = 32;
        int chunkHeight = 256;
        int chunkDepth = 32;
        int viewDistance = 10;
        ChunkManager chunkManager(chunkWidth, chunkHeight, chunkDepth, viewDistance);
        chunkManager.updatePlayerPosition(camera.getPosition());

        Renderer renderer;
        renderer.setCamera(&camera);
        renderer.initShaders();
        renderer.loadTexture("../assets/textures/atlas.png");
        renderer.setSkyboxData(skyBox.GetVertices());

        double lastFrame = glfwGetTime();

        while (!window.shouldClose()) {
            double currentFrame = glfwGetTime();
            double deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            window.pollEvents();

            glm::vec3 cameraPos = camera.getPosition();
            chunkManager.updatePlayerPosition(cameraPos);

            std::tuple<glm::ivec2, std::vector<float>, std::vector<unsigned int>> item;
            while (chunkManager.getRenderQueue().tryPop(item)) {
                const auto& [chunkPos, vertices, indices] = item;
                if (!vertices.empty() && !indices.empty()) {
                    renderer.addChunk(chunkPos, vertices, indices);
                } else {
                    renderer.removeChunk(chunkPos);
                }
            }

            chunkManager.clearChunkChanges();
            renderer.processChunkUpdates();
            renderer.draw();

            window.swapBuffers();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}