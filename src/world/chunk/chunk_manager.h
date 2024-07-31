#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "chunk.h"
#include "../../utils/hash.h"

class ChunkManager {
public:
    ChunkManager(int chunkWidth, int chunkHeight, int chunkDepth, int viewDistance);
    void updatePlayerPosition(const glm::vec3& playerPos);

    std::vector<float> getVertexData();
    std::vector<unsigned int> getIndexData();

private:
    int chunkWidth, chunkHeight, chunkDepth;
    int viewDistance;
    glm::vec3 playerPosition;
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>> chunks;
    
    void loadChunks();
    void unloadChunks();
    glm::ivec2 worldToChunkCoords(const glm::vec3& worldPos);

};

#endif // CHUNKMANAGER_H
 