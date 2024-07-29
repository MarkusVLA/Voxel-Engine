#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>
#include <vector>
#include "types.h"

class Voxel {
public:
    Voxel(glm::vec3 position, int type);

    glm::vec3 getPosition() const;
    int getType() const;

    std::vector<float> getVertexData(const glm::vec3& offset) const;
    std::vector<unsigned int> getIndexData(unsigned int baseIndex) const;

private:
    glm::vec3 position;
    int type;
};

#endif // VOXEL_H
 