#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>

class Voxel {
public:
    Voxel(glm::vec3 position, int type);
    glm::vec3 getPosition() const;
    int getType() const;

private:
    glm::vec3 position;
    int type;
};

#endif // VOXEL_H
