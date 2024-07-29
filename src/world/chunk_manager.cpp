#include "chunk_manager.h"
#include <iostream>

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int worldWidth, int worldHeight)
    : chunkWidth(chunkWidth), chunkHeight(chunkHeight), chunkDepth(chunkDepth), worldWidth(worldWidth), worldHeight(worldHeight) {
    generateWorld();
}

ChunkManager::~ChunkManager() {
    // Cleanup if necessary
}

void ChunkManager::generateWorld() {
    for (int x = 0; x < worldWidth; ++x) {
        for (int z = 0; z < worldHeight; ++z) {
            glm::ivec2 chunkIndex = glm::ivec2(x, z);
            chunks.emplace(chunkIndex, Chunk(chunkWidth, chunkHeight, chunkDepth, glm::vec2(chunkIndex)));
            std::cout << "Loaded chunk at: " << chunkIndex.x << ", " << chunkIndex.y << std::endl;
        }
    }
}

std::vector<float> ChunkManager::getVertexData() {
    std::vector<float> vertices;
    for (const auto& pair : chunks) {
        const auto& chunk = pair.second;
        std::vector<float> chunkVertices = chunk.getVertexData();
        vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
    }
    return vertices;
}

std::vector<unsigned int> ChunkManager::getIndexData() {
    std::vector<unsigned int> indices;
    unsigned int baseIndex = 0;
    for (const auto& pair : chunks) {
        const auto& chunk = pair.second;
        std::vector<unsigned int> chunkIndices = chunk.getIndexData();
        for (unsigned int index : chunkIndices) {
            indices.push_back(index + baseIndex);
        }
        baseIndex += chunk.getVertexData().size() / 9; // Update based on the vertex size (now 9 floats per vertex)
    }
    return indices;
}
