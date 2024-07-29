#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm/glm.hpp>
#include "voxel/voxel.h"

class Chunk {
public:
    Chunk(int width, int height, int depth, glm::vec2 index);

    std::vector<float> getVertexData() const;
    std::vector<unsigned int> getIndexData() const;

    glm::vec2 getIndex() const;
    bool operator==(const Chunk& other) const;

private:
    void generateTerrain();
    int generateHeight(int x, int z) const;

    int width, height, depth;
    glm::vec2 index_;
    std::vector<Voxel> voxels;
};

#endif // CHUNK_H
