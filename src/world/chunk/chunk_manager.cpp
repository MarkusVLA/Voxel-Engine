#include "chunk_manager.h"
#include <iostream>
#include <cmath>

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance)
    : chunkWidth(chunkWidth), chunkHeight(chunkHeight), chunkDepth(chunkDepth), viewDistance(viewDistance),
      running(true), lastLoadedCenterChunk(0, 0) {
    startWorkers(2);
}

ChunkManager::~ChunkManager() {
    running = false;
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
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
    expandLoadedArea(playerChunk);
}

void ChunkManager::unloadChunks() {
    glm::ivec2 playerChunk = worldToChunkCoords(playerPosition);
    std::lock_guard<std::mutex> lock(chunksMutex);
    for (auto it = chunks.begin(); it != chunks.end();) {
        glm::ivec2 chunkPos = it->first;
        if (!isChunkInLoadDistance(chunkPos, playerChunk)) {
            renderQueue.push({chunkPos, {}, {}, {}, {}});  // Empty vectors for both solid and water meshes
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}

void ChunkManager::updatePlayerPosition(const glm::vec3& position) {
    playerPosition = position;
    glm::ivec2 currentChunk = worldToChunkCoords(position);
    if (currentChunk != lastLoadedCenterChunk) {
        expandLoadedArea(currentChunk);
        lastLoadedCenterChunk = currentChunk;
    }
    unloadChunks();
}

void ChunkManager::startWorkers(size_t numWorkers) {
    for (size_t i = 0; i < numWorkers; ++i) {
        workers.emplace_back(&ChunkManager::workerFunction, this);
    }
}

void ChunkManager::workerFunction() {
    while (running) {
        std::function<void()> task;
        if (taskQueue.tryPop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
}

std::shared_ptr<Chunk> ChunkManager::getChunk(const glm::ivec2& chunkPos) {
    std::lock_guard<std::mutex> lock(chunksMutex);
    auto it = chunks.find(chunkPos);
    if (it != chunks.end()) {
        return it->second;
    }
    return nullptr;
}

ThreadSafeQueue<std::tuple<glm::ivec2, std::vector<float>, std::vector<unsigned int>, std::vector<float>, std::vector<unsigned int>>>& ChunkManager::getRenderQueue() {
    return renderQueue;
}

void ChunkManager::expandLoadedArea(const glm::ivec2& newCenterChunk) {
    int start_x = newCenterChunk.x - viewDistance;
    int end_x = newCenterChunk.x + viewDistance;
    int start_z = newCenterChunk.y - viewDistance;
    int end_z = newCenterChunk.y + viewDistance;
    for (int x = start_x; x <= end_x; ++x) {
        for (int z = start_z; z <= end_z; ++z) {
            glm::ivec2 chunkPos(x, z);
            std::lock_guard<std::mutex> lock(chunksMutex);
            if (chunks.find(chunkPos) == chunks.end()) {
                taskQueue.push([this, chunkPos] {
                    auto chunk = std::make_shared<Chunk>(chunkWidth, chunkHeight, chunkDepth, chunkPos);
                    auto [solidVertices, solidIndices, waterVertices, waterIndices] = chunk->getMesh();
                    {
                        std::lock_guard<std::mutex> lock(chunksMutex);
                        chunks[chunkPos] = chunk;
                    }
                    renderQueue.push({chunkPos, std::move(solidVertices), std::move(solidIndices), std::move(waterVertices), std::move(waterIndices)});
                });
            }
        }
    }
}

bool ChunkManager::isChunkInLoadDistance(const glm::ivec2& chunkPos, const glm::ivec2& centerChunk) {
    glm::ivec2 diff = glm::abs(chunkPos - centerChunk);
    return diff.x <= viewDistance && diff.y <= viewDistance;
}

int ChunkManager::getLoadedChunksCount() const {
    return chunks.size();
}