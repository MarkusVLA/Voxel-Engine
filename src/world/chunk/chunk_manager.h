#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include "chunk.h"
#include "../../utils/hash.h"
#include "../../utils/thread_safe_queue.h"

class ChunkManager {
public:
    ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance);
    ~ChunkManager();

    void updatePlayerPosition(const glm::vec3& playerPos);
    std::shared_ptr<Chunk> getChunk(const glm::ivec2& chunkPos);
    ThreadSafeQueue<std::tuple<glm::ivec2, std::vector<float>, std::vector<unsigned int>>>& getRenderQueue();

private:
    struct ChunkTask {
        glm::ivec2 chunkPos;
        float priority;
        
        bool operator<(const ChunkTask& other) const {
            return priority > other.priority; // Higher priority first
        }
    };

    int chunkWidth, chunkHeight, chunkDepth;
    int viewDistance;
    glm::vec3 playerPosition;
    std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>, IVec2Hash> chunks;
    std::vector<std::thread> workers;
    std::mutex chunksMutex;
    std::atomic_bool running;
    ThreadSafeQueue<std::function<void()>> taskQueue;
    ThreadSafeQueue<std::tuple<glm::ivec2, std::vector<float>, std::vector<unsigned int>>> renderQueue;
    glm::ivec2 lastLoadedCenterChunk;

    void loadChunks();
    void unloadChunks();
    glm::ivec2 worldToChunkCoords(const glm::vec3& worldPos);
    void startWorkers(size_t numWorkers);
    void workerFunction();
    void expandLoadedArea(const glm::ivec2& newCenterChunk);
    bool isChunkInLoadDistance(const glm::ivec2& chunkPos, const glm::ivec2& centerChunk);
    float calculateChunkPriority(const glm::ivec2& chunkPos, const glm::ivec2& playerChunk);
};

#endif // CHUNKMANAGER_H