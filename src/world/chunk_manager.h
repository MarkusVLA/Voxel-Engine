#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "chunk.h"
#include "../utils/hash.h"  // Ensure this is included

class ChunkManager {
public:
    ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance);
    ~ChunkManager();

    void update(const glm::vec3& cameraPosition);
    bool hasChunksChanged() const;
    std::vector<float> getVertexData();
    std::vector<unsigned int> getIndexData();
    void getNewChunkData(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:
    int chunkWidth;
    int chunkHeight;
    int chunkDepth;
    int viewDistance;
    bool chunksChanged;
    glm::ivec2 lastCameraChunk;

    std::unordered_map<glm::ivec2, Chunk> chunks;
    std::vector<glm::ivec2> newChunkIndices;

    std::mutex chunkMutex;
    std::condition_variable chunkCondition;
    std::thread chunkLoaderThread;
    bool running;

    void loadChunk(const glm::ivec2& index);
    void unloadChunk(const glm::ivec2& index);
    void chunkLoader();
    void loadChunksAround(const glm::ivec2& cameraChunk);
};

#endif // CHUNKMANAGER_H
