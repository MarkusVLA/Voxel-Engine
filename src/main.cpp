#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/renderer/renderer.h"
#include "engine/window/window.h"
#include "engine/input_listener/input_listener.h"
#include "engine/camera/camera.h"
#include "world/skybox.h"
#include "world/chunk/chunk_manager.h"

int main() {
    Window window(1920, 1200, "OpenGL Window");
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 128.0f, 0.0f), -90.0f, 0.0f);
    InputListener::setCamera(&camera);
    SkyBox skyBox;

    int chunkWidth = 16;
    int chunkHeight = 256;
    int chunkDepth = 16;
    int viewDistance = 4;

    ChunkManager chunkManager(chunkWidth, chunkHeight, chunkDepth, viewDistance);

    Renderer renderer;
    renderer.setCamera(&camera);
    renderer.loadTexture("../assets/textures/atlas.png");
    renderer.setSkyboxData(skyBox.GetVertices());

    chunkManager.updatePlayerPosition(camera.getPosition());
    for (const auto& chunk : chunkManager.getChunks()) {
        glm::ivec2 chunkPos = chunk.first;
        std::vector<float> vertices = chunk.second->getVertexData();
        std::vector<unsigned int> indices = chunk.second->getIndexData();
        renderer.addChunk(chunkPos, vertices, indices);
    }

    while (!window.shouldClose()) {
        window.pollEvents();
        chunkManager.updatePlayerPosition(camera.getPosition());
        for (const auto& chunkPos : chunkManager.getNewlyLoadedChunks()) {
            const auto& chunk = chunkManager.getChunk(chunkPos);
            if (chunk) {
                std::vector<float> vertices = chunk->getVertexData();
                std::vector<unsigned int> indices = chunk->getIndexData();
                renderer.addChunk(chunkPos, vertices, indices);
            }
        }
        for (const auto& chunkPos : chunkManager.getUnloadedChunks()) {
            renderer.removeChunk(chunkPos);
        }

        chunkManager.clearChunkChanges();
        renderer.draw();
        window.swapBuffers();
    }

    return 0;
}