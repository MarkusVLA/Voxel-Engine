#include "chunk_manager.h"
#include <iostream>
#include <cmath>

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance)
    : chunkWidth(chunkWidth), chunkHeight(chunkHeight), chunkDepth(chunkDepth), viewDistance(viewDistance), running(true) {
    workerThread = std::thread(&ChunkManager::worker, this);
}

ChunkManager::~ChunkManager() {
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
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
                newlyLoadedChunks.push_back(chunkPos);
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
            unloadedChunks.push_back(chunkPos);
            std::cout << "Unloaded chunk at: " << chunkPos.x << ", " << chunkPos.y << std::endl;
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}

void ChunkManager::updatePlayerPosition(const glm::vec3& playerPos) {
    std::lock_guard<std::mutex> lock(chunksMutex);
    playerPosition = playerPos;
}

void ChunkManager::worker() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(chunksMutex);
            loadChunks();
            unloadChunks();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust sleep time as needed
    }
}

std::vector<float> ChunkManager::getVertexData() {
    std::lock_guard<std::mutex> lock(chunksMutex);
    std::vector<float> vertices;
    for (const auto& pair : chunks) {
        const auto& chunk = pair.second;
        std::vector<float> chunkVertices = chunk->getVertexData();
        vertices.insert(vertices.end(), chunkVertices.begin(), chunkVertices.end());
    }
    return vertices;
}

std::vector<unsigned int> ChunkManager::getIndexData() {
    std::lock_guard<std::mutex> lock(chunksMutex);
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

const std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, IVec2Hash>& ChunkManager::getChunks() const {
    return chunks;
}

std::vector<glm::ivec2> ChunkManager::getNewlyLoadedChunks() {
    std::lock_guard<std::mutex> lock(chunksMutex);
    return newlyLoadedChunks;
}

std::vector<glm::ivec2> ChunkManager::getUnloadedChunks() {
    std::lock_guard<std::mutex> lock(chunksMutex);
    return unloadedChunks;
}

void ChunkManager::clearChunkChanges() {
    std::lock_guard<std::mutex> lock(chunksMutex);
    newlyLoadedChunks.clear();
    unloadedChunks.clear();
}

std::unique_ptr<Chunk>& ChunkManager::getChunk(const glm::ivec2& chunkPos) {
    std::lock_guard<std::mutex> lock(chunksMutex);
    auto it = chunks.find(chunkPos);
    if (it != chunks.end()) {
        return it->second;
    }
    static std::unique_ptr<Chunk> nullChunk;
    return nullChunk;
}
