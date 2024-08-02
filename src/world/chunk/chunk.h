#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include "../voxel/voxel.h"
#include "../voxel/types.h"
#include "../../utils/perlin.h"
#include "../../utils/pop_count.h"
#include "terrain_generator/terrain_generator.h"



class Chunk {
public:
    Chunk(int width, int height, int depth, glm::vec2 index);
    ~Chunk();

    std::tuple<std::vector<float>, std::vector<unsigned int>,
               std::vector<float>, std::vector<unsigned int>> getMesh() const;
    Voxel* getVoxel(const glm::vec3& pos) const;
    void setVoxel(const glm::vec3& pos, VoxelType type);
    glm::vec2 getIndex() const;
    bool operator==(const Chunk& other) const;

private:
    void addVoxelMesh(Voxel* voxel, const glm::vec3& offset, uint8_t faceFlags,
                      std::vector<float>& vertices, std::vector<unsigned int>& indices,
                      unsigned int& baseIndex) const;
    glm::vec3 indexToCoords(int index) const;
    unsigned int coordsToIndex(glm::vec3 coords) const;
    uint8_t getFaceFlags(glm::vec3 pos) const;

    int width, height, depth;
    glm::vec2 index_;
    std::vector<Voxel*> voxels;
    TerrainGenerator terrainGenerator;
};

#endif // CHUNK_H
