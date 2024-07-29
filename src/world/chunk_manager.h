#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <vector>
#include <glm/glm.hpp>
#include "chunk.h"

class ChunkManager {
public:
    ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth);

    void addChunk(const glm::vec2& index);
    std::vector<float> getVertexData() const;
    std::vector<unsigned int> getIndexData() const;

private:
    int chunkWidth;
    int chunkHeight;
    int chunkDepth;
    std::vector<Chunk> chunks;
};

#endif // CHUNKMANAGER_H
