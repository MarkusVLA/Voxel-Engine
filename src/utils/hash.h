#ifndef HASH_H
#define HASH_H

#include <glm/glm.hpp>
#include <functional>

struct IVec2Hash {
    std::size_t operator()(const glm::ivec2& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

#endif // HASH_H