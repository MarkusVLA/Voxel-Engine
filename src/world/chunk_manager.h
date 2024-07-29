#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "chunk.h"
#include "../utils/hash.h"  // Ensure this is included

class ChunkManager {
public:
    ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int worldWidth, int worldHeight);
    ~ChunkManager();

    std::vector<float> getVertexData();
    std::vector<unsigned int> getIndexData();

private:
    int chunkWidth;
    int chunkHeight;
    int chunkDepth;
    int worldWidth;
    int worldHeight;

    std::unordered_map<glm::ivec2, Chunk> chunks;

    void generateWorld();
};

#endif // CHUNKMANAGER_H
