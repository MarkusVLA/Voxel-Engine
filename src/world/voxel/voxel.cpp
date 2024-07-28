#include "voxel.h"

Voxel::Voxel(glm::vec3 position, int type) : position(position), type(type) {}

glm::vec3 Voxel::getPosition() const {
    return position;
}

int Voxel::getType() const {
    return type;
}
