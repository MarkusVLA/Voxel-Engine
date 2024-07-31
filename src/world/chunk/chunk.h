#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm/glm.hpp>
#include "../voxel/voxel.h"
#include "../voxel/types.h"
#include "../../utils/perlin.h"

class Chunk {
public:
    Chunk(int width, int height, int depth, glm::vec2 index);

    std::vector<float> getVertexData() const;
    std::vector<unsigned int> getIndexData() const;
    std::vector<float> getMesh() const; // Move to chunkmesh
    Voxel getPosTile(glm::vec3 pos) const;
    
    glm::vec2 getIndex() const;
    bool operator==(const Chunk& other) const;


    glm::vec3 indexToCoords(int index) const;
    unsigned int coordsToIndex(glm::vec3 coords) const;



private:
    void generateTerrain();
    int generateHeight(int x, int z) const;


    int width, height, depth;
    glm::vec2 index_;
    std::vector<Voxel> voxels;
    Perlin perlinNoise; 
};

#endif // CHUNK_H