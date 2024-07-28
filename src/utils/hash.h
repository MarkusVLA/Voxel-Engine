#ifndef HASH_H
#define HASH_H

#include <glm/glm.hpp>
#include <functional>

namespace std {
    template <>
    struct hash<glm::ivec2> {
        size_t operator()(const glm::ivec2& k) const {
            return ((hash<int>()(k.x) ^ (hash<int>()(k.y) << 1)) >> 1);
        }
    };
}

#endif // HASH_H
