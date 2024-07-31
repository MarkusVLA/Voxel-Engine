#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include "chunk.h"
#include "../../utils/hash.h"

class ChunkManager {
public:
    ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance);
    ~ChunkManager();

    void updatePlayerPosition(const glm::vec3& playerPos);
    std::vector<float> getVertexData();
    std::vector<unsigned int> getIndexData();
    const std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, IVec2Hash>& getChunks() const;
    std::vector<glm::ivec2> getNewlyLoadedChunks();
    std::vector<glm::ivec2> getUnloadedChunks();
    void clearChunkChanges();
    std::unique_ptr<Chunk>& getChunk(const glm::ivec2& chunkPos);

private:
    int chunkWidth, chunkHeight, chunkDepth;
    int viewDistance;
    glm::vec3 playerPosition;
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, IVec2Hash> chunks;
    std::vector<glm::ivec2> newlyLoadedChunks;
    std::vector<glm::ivec2> unloadedChunks;
    
    std::thread workerThread;
    std::mutex chunksMutex;
    std::atomic_bool running;

    void loadChunks();
    void unloadChunks();
    void worker();
    glm::ivec2 worldToChunkCoords(const glm::vec3& worldPos);
};

#endif // CHUNKMANAGER_H
