#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include "../voxel/voxel.h"
#include "../voxel/types.h"
#include "../../utils/perlin.h"
#include "../../utils/pop_count.h"

class Chunk {
public:
    Chunk(int width, int height, int depth, glm::vec2 index);
    ~Chunk();

    // New function to get both vertex and index data
    std::tuple<std::vector<float>, std::vector<unsigned int>> getMesh() const;

    Voxel* getVoxel(const glm::vec3& pos) const;
    void setVoxel(const glm::vec3& pos, VoxelType type);
    glm::vec2 getIndex() const;
    bool operator==(const Chunk& other) const;

private:
    glm::vec3 indexToCoords(int index) const;
    unsigned int coordsToIndex(glm::vec3 coords) const;
    void generateTerrain();
    int generateHeight(int x, int z) const;
    uint8_t getFaceFlags(glm::vec3 pos) const;

    int width, height, depth;
    glm::vec2 index_;
    std::vector<Voxel*> voxels;
    Perlin perlinNoise;
};

#endif // CHUNK_H