#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../global.h"


class Frustum {
public:
    Frustum(const glm::mat4& m) {
        float t;

        // Left plane
        planes[0].x = m[0][3] + m[0][0];
        planes[0].y = m[1][3] + m[1][0];
        planes[0].z = m[2][3] + m[2][0];
        planes[0].w = m[3][3] + m[3][0];

        // Right plane
        planes[1].x = m[0][3] - m[0][0];
        planes[1].y = m[1][3] - m[1][0];
        planes[1].z = m[2][3] - m[2][0];
        planes[1].w = m[3][3] - m[3][0];

        // Top plane
        planes[2].x = m[0][3] - m[0][1];
        planes[2].y = m[1][3] - m[1][1];
        planes[2].z = m[2][3] - m[2][1];
        planes[2].w = m[3][3] - m[3][1];

        // Bottom plane
        planes[3].x = m[0][3] + m[0][1];
        planes[3].y = m[1][3] + m[1][1];
        planes[3].z = m[2][3] + m[2][1];
        planes[3].w = m[3][3] + m[3][1];

        // Near plane
        planes[4].x = m[0][3] + m[0][2];
        planes[4].y = m[1][3] + m[1][2];
        planes[4].z = m[2][3] + m[2][2];
        planes[4].w = m[3][3] + m[3][2];

        // Far plane
        planes[5].x = m[0][3] - m[0][2];
        planes[5].y = m[1][3] - m[1][2];
        planes[5].z = m[2][3] - m[2][2];
        planes[5].w = m[3][3] - m[3][2];

        // Normalize planes
        for(int i = 0; i < 6; i++) {
            t = sqrt(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
            planes[i] /= t;
        }
    }

    bool isChunkVisible(const glm::ivec2& chunkPos) const {
        glm::vec3 center(chunkPos.x * 16.0f + 8.0f, 128.0f, chunkPos.y * 16.0f + 8.0f);
        
        // Radius of a sphere that encloses a 16x256x16 cuboid
        float radius = sqrt(8.0f*8.0f + 128.0f*128.0f + 8.0f*8.0f);

        float margin = 2.0f;

        for(int i = 0; i < 6; i++) {
            if(planes[i].x * center.x + planes[i].y * center.y + planes[i].z * center.z + planes[i].w <= -(radius + margin)) {
                return false;
            }
        }
        return true;
    }

private:
    glm::vec4 planes[6]; // Left, Right, Top, Bottom, Near, Far
};