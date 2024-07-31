#include "chunk_manager.h"
#include <iostream>
#include <cmath>

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance)
    : chunkWidth(chunkWidth), chunkHeight(chunkHeight), chunkDepth(chunkDepth), viewDistance(viewDistance) {
}

void ChunkManager::updatePlayerPosition(const glm::vec3& playerPos) {
    playerPosition = playerPos;
    loadChunks();
    unloadChunks();
}

glm::ivec2 ChunkManager::worldToChunkCoords(const glm::vec3& worldPos) {
    return glm::ivec2(
        static_cast<int>(std::floor(worldPos.x / chunkWidth)),
        static_cast<int>(std::floor(worldPos.z / chunkDepth))
    );
}

void ChunkManager::loadChunks() {
    glm::ivec2 playerChunk = worldToChunkCoords(playerPosition);
    
    for (int x = -viewDistance; x <= viewDistance; ++x) {
        for (int z = -viewDistance; z <= viewDistance; ++z) {
            glm::ivec2 chunkPos = playerChunk + glm::ivec2(x, z);
            
            if (chunks.find(chunkPos) == chunks.end()) {
                chunks[chunkPos] = std::make_unique<Chunk>(chunkWidth, chunkHeight, chunkDepth, glm::vec2(chunkPos));
                std::cout << "Loaded chunk at: " << chunkPos.x << ", " << chunkPos.y << std::endl;
            }
        }
    }
}

void ChunkManager::unloadChunks() {
    glm::ivec2 playerChunk = worldToChunkCoords(playerPosition);
    
    for (auto it = chunks.begin(); it != chunks.end(); ) {
        glm::ivec2 chunkPos = it->first;
        glm::ivec2 diff = glm::abs(chunkPos - playerChunk);
        
        if (diff.x > viewDistance || diff.y > viewDistance) {
            std::cout << "Unloaded chunk at: " << chunkPos.x << ", " << chunkPos.y << std::endl;
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<float> ChunkManager::getVertexData() {
    std::vector<float> vertices;
    for (const auto& pair : chunks) {
        const auto& chunk = pair.second;
        std::vector<float> chunkVertices = chunk->getVertexData();
        vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
    }
    return vertices;
}

std::vector<unsigned int> ChunkManager::getIndexData() {
    std::vector<unsigned int> indices;
    unsigned int baseIndex = 0;
    for (const auto& pair : chunks) {
        const auto& chunk = pair.second;
        std::vector<unsigned int> chunkIndices = chunk->getIndexData();
        for (unsigned int index : chunkIndices) {
            indices.push_back(index + baseIndex);
        }
        baseIndex += chunk->getVertexData().size() / 9;
    }
    return indices;
}