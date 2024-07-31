
#ifndef CHUNK_LOAD_COMPARATOR_H
#define CHUNK_LOAD_COMPARATOR_H
#include <iostream>
#include <glm/glm.hpp>

struct ChunkLoadComparator {
    bool operator()(const std::pair<float, glm::ivec2>& lhs, const std::pair<float, glm::ivec2>& rhs) const {
        return lhs.first > rhs.first;
    }
};


#endif // CHUNK_LOAD_COMPARATOR_H
