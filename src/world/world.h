#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "voxel/voxel.h"

class World {
public:
    World(int width, int height, int depth);
    void generateTerrain();
    std::vector<float> getVertexData() const;
    std::vector<unsigned int> getIndexData() const;

private:
    int width;
    int height;
    int depth;
    std::vector<Voxel> voxels;

    int generateHeight(int x, int z) const;
};

#endif // WORLD_H
