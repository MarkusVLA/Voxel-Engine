#include "voxel.h"

Voxel::Voxel(glm::vec3 position, int type, bool isXShaped) : position(position), type(type), isXShaped(isXShaped) {
    if (type == WATER) translucent = true;
}

glm::vec3 Voxel::getPosition() const {
    return position;
}

int Voxel::getType() const {
    return type;
}

bool Voxel::getIsXShaped() const {
    return isXShaped;
}

bool Voxel::isTranslucent() const {
    return translucent;
}

std::vector<float> Voxel::getVertexData(const glm::vec3& offset, uint8_t faceFlags) const {
    std::vector<float> vertices;
    glm::vec3 offsetPosition = position + offset;


    // Front face
    if (faceFlags & FACE_FRONT) {
        vertices.insert(vertices.end(), {
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f, static_cast<float>(type)
        });
    }

    // Back face
    if (faceFlags & FACE_BACK) {
        vertices.insert(vertices.end(), {
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f, static_cast<float>(type)
        });
    }

    // Left face
    if (faceFlags & FACE_LEFT) {
        vertices.insert(vertices.end(), {
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, static_cast<float>(type)
        });
    }

    // Right face
    if (faceFlags & FACE_RIGHT) {
        vertices.insert(vertices.end(), {
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f, static_cast<float>(type)
        });
    }

    // Top face
    if (faceFlags & FACE_TOP) {
        vertices.insert(vertices.end(), {
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, static_cast<float>(type)
        });
    }

    // Bottom face
    if (faceFlags & FACE_BOTTOM) {
        vertices.insert(vertices.end(), {
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, static_cast<float>(type)
        });
    }

   if (faceFlags & FACE_DIAGONAL_1) {
        vertices.insert(vertices.end(), {
            // Bottom-left to top-right diagonal
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  1.0f, 1.0f, 1.0f, static_cast<float>(type)
        });
    }

    if (faceFlags & FACE_DIAGONAL_2) {
        vertices.insert(vertices.end(), {
            // Bottom-right to top-left diagonal (corrected)
            offsetPosition.x + 0.5f, offsetPosition.y - 0.5f, offsetPosition.z - 0.5f,  0.0f, 0.0f,  -1.0f, 1.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y - 0.5f, offsetPosition.z + 0.5f,  1.0f, 0.0f,  -1.0f, 1.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x - 0.5f, offsetPosition.y + 0.5f, offsetPosition.z + 0.5f,  1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, static_cast<float>(type),
            offsetPosition.x + 0.5f, offsetPosition.y + 0.5f, offsetPosition.z - 0.5f,  0.0f, 1.0f,  -1.0f, 1.0f, 1.0f, static_cast<float>(type)
        });
    }

    return vertices;
}

std::vector<unsigned int> Voxel::getIndexData(unsigned int baseIndex, uint8_t faceFlags) const {
    std::vector<unsigned int> indices;
    unsigned int faceOffset = 0;

    const std::vector<unsigned int> faceIndices = {0, 1, 2, 2, 3, 0};
    
    // Add indices based on face flags
    if (faceFlags & FACE_FRONT) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
        faceOffset += 4;
    }
    if (faceFlags & FACE_BACK) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
        faceOffset += 4;
    }
    if (faceFlags & FACE_LEFT) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
        faceOffset += 4;
    }
    if (faceFlags & FACE_RIGHT) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
        faceOffset += 4;
    }
    if (faceFlags & FACE_TOP) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
        faceOffset += 4;
    }
    if (faceFlags & FACE_BOTTOM) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
    }

   if (faceFlags & FACE_DIAGONAL_1) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
        faceOffset += 4;
    }

    if (faceFlags & FACE_DIAGONAL_2) {
        for (unsigned int idx : faceIndices) {
            indices.push_back(baseIndex + faceOffset + idx);
        }
    }

    return indices;
}
