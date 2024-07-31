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

    std::vector<float> getVertexData(const glm::vec3& offset, uint8_t faceFlags) const;
    std::vector<unsigned int> getIndexData(unsigned int baseIndex, uint8_t faceFlags) const;
    
private:
    glm::vec3 position;
    int type;
};


enum FaceFlags {
    FACE_FRONT  = 1 << 0,
    FACE_BACK   = 1 << 1,
    FACE_LEFT   = 1 << 2,
    FACE_RIGHT  = 1 << 3,
    FACE_TOP    = 1 << 4,
    FACE_BOTTOM = 1 << 5
};

#endif // VOXEL_H
 