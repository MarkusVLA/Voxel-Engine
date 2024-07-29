#include "chunk_manager.h"

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth)
    : chunkWidth(chunkWidth), chunkHeight(chunkHeight), chunkDepth(chunkDepth) {}

void ChunkManager::addChunk(const glm::vec2& index) {
    chunks.emplace_back(chunkWidth, chunkHeight, chunkDepth, index);
}

std::vector<float> ChunkManager::getVertexData() const {
    std::vector<float> vertices;
    for (const auto& chunk : chunks) {
        std::vector<float> chunkVertices = chunk.getVertexData();
        vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
    }
    return vertices;
}

std::vector<unsigned int> ChunkManager::getIndexData() const {
    std::vector<unsigned int> indices;
    unsigned int baseIndex = 0;

    for (const auto& chunk : chunks) {
        std::vector<unsigned int> chunkIndices = chunk.getIndexData();
        for (unsigned int index : chunkIndices) {
            indices.push_back(index + baseIndex);
        }
        baseIndex += chunk.getVertexData().size() / 8; // Assuming 8 floats per vertex
    }
    return indices;
}
