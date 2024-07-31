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
    // Create window (This also initializes GLFW, creates OpenGL context, and initializes GLAD)
    Window window(1920, 1200, "OpenGL Window");

    // Set up camera
    Camera camera(glm::vec3(0.0f, 128.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    InputListener::setCamera(&camera);

    // Set up skybox
    SkyBox skyBox;

    // Set up chunk manager
    int chunkWidth = 16;
    int chunkHeight = 256;
    int chunkDepth = 16;
    int viewDistance = 1;
    ChunkManager chunkManager(chunkWidth, chunkHeight, chunkDepth, viewDistance);

    // Set up renderer
    Renderer renderer;
    renderer.setCamera(&camera);
    renderer.initShaders();  
    renderer.loadTexture("../assets/textures/atlas.png");
    renderer.setSkyboxData(skyBox.GetVertices());

    chunkManager.updatePlayerPosition(camera.getPosition());
    for (const auto& chunk : chunkManager.getChunks()) {
        glm::ivec2 chunkPos = chunk.first;
        std::vector<float> vertices = chunk.second->getVertexData();
        std::vector<unsigned int> indices = chunk.second->getIndexData();
        renderer.addChunk(chunkPos, vertices, indices);
    }

    // Main loop
    while (!window.shouldClose()) {
        window.pollEvents();

        // Update chunk loading based on camera position
        chunkManager.updatePlayerPosition(camera.getPosition());

        // Handle newly loaded chunks
        for (const auto& chunkPos : chunkManager.getNewlyLoadedChunks()) {
            const auto& chunk = chunkManager.getChunk(chunkPos);
            if (chunk) {
                std::vector<float> vertices = chunk->getVertexData();
                std::vector<unsigned int> indices = chunk->getIndexData();
                renderer.addChunk(chunkPos, vertices, indices);
            }
        }

        // Handle unloaded chunks
        for (const auto& chunkPos : chunkManager.getUnloadedChunks()) {
            renderer.removeChunk(chunkPos);
        }

        chunkManager.clearChunkChanges();

        // Process chunk updates and draw
        renderer.processChunkUpdates();
        renderer.draw();

        window.swapBuffers();
    }

    return 0;
}