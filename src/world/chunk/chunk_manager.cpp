#include "chunk_manager.h"
#include <cstdlib>
#include <iostream>
#include <shared_mutex>
#include <cmath>
#include <queue>

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance)
    : chunkWidth(chunkWidth), chunkHeight(chunkHeight), chunkDepth(chunkDepth), viewDistance(viewDistance),
      running(false), lastLoadedCenterChunk(2, 2) {
}

void ChunkManager::init(unsigned int s){
    seed = s;
    startWorkers(4);
    running = true;
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
            renderQueue.push({chunkPos, {}, {}, {}, {}});
            chunkStates.erase(chunkPos);
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
    std::priority_queue<ChunkTask> chunkTaskQueue;

    int start_x = newCenterChunk.x - viewDistance;
    int end_x = newCenterChunk.x + viewDistance;
    int start_z = newCenterChunk.y - viewDistance;
    int end_z = newCenterChunk.y + viewDistance;

    for (int x = start_x; x <= end_x; ++x) {
        for (int z = start_z; z <= end_z; ++z) {
            glm::ivec2 chunkPos(x, z);
            float distance = glm::length(glm::vec2(chunkPos - newCenterChunk));
            chunkTaskQueue.push({chunkPos, distance});
        }
    }

    while (!chunkTaskQueue.empty()) {
        ChunkTask task = chunkTaskQueue.top();
        chunkTaskQueue.pop();

        std::lock_guard<std::mutex> lock(chunksMutex);
        if (chunks.find(task.chunkPos) == chunks.end()) {
            chunkStates[task.chunkPos] = ChunkGenerationState::QUEUED;
            taskQueue.push([this, task] {
                chunkStates[task.chunkPos] = ChunkGenerationState::GENERATING;
                auto chunk = std::make_shared<Chunk>(chunkWidth, chunkHeight, chunkDepth, task.chunkPos, this, seed);
                {
                    std::lock_guard<std::mutex> lock(chunksMutex);
                    chunks[task.chunkPos] = chunk;
                    chunkStates[task.chunkPos] = ChunkGenerationState::GENERATED;
                }
            });
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

void ChunkManager::updateChunks() {
    std::vector<ChunkTask> chunksToUpdate;
    glm::ivec2 playerChunk = worldToChunkCoords(playerPosition);

    {
        std::lock_guard<std::mutex> lock(chunksMutex);
        for (auto& [chunkPos, chunk] : chunks) {
            if (chunkStates[chunkPos] == ChunkGenerationState::GENERATED) {
                float distance = glm::length(glm::vec2(chunkPos - playerChunk));
                chunksToUpdate.push_back({chunkPos, distance}); // Closer chunks have smaller distance
            }
        }
    }

    std::sort(chunksToUpdate.begin(), chunksToUpdate.end());

    for (const auto& task : chunksToUpdate) {
        std::lock_guard<std::mutex> lock(chunksMutex);
        auto chunk = chunks[task.chunkPos];
        bool allNeighborsGenerated = true;

        for (const auto& offset : {glm::ivec2(1, 0), glm::ivec2(-1, 0), glm::ivec2(0, 1), glm::ivec2(0, -1)}) {
            glm::ivec2 neighborPos = task.chunkPos + offset;
            if (chunks.find(neighborPos) == chunks.end() || chunkStates[neighborPos] == ChunkGenerationState::QUEUED) {
                allNeighborsGenerated = false;
                break;
            }
        }

        if (allNeighborsGenerated) {
            chunkStates[task.chunkPos] = ChunkGenerationState::MESHING;
            taskQueue.push([this, chunkPos = task.chunkPos, chunk] {
                // Place outside voxels
                chunk->placeOutsideVoxels();

                // Generate mesh for this chunk
                auto [solidVertices, solidIndices, waterVertices, waterIndices] = chunk->getMesh();
                renderQueue.push({chunkPos, std::move(solidVertices), std::move(solidIndices), std::move(waterVertices), std::move(waterIndices)});

                // Update neighboring chunk meshes
                for (const auto& offset : {glm::ivec2(1, 0), glm::ivec2(-1, 0), glm::ivec2(0, 1), glm::ivec2(0, -1)}) {
                    glm::ivec2 neighborPos = chunkPos + offset;
                    auto neighborChunk = getChunk(neighborPos);
                    if (neighborChunk) {
                        auto [nSolidVertices, nSolidIndices, nWaterVertices, nWaterIndices] = neighborChunk->getMesh();
                        renderQueue.push({neighborPos, std::move(nSolidVertices), std::move(nSolidIndices), std::move(nWaterVertices), std::move(nWaterIndices)});
                    }
                }

                std::lock_guard<std::mutex> lock(chunksMutex);
                chunkStates[chunkPos] = ChunkGenerationState::MESHED;
            });
        }
    }
}
