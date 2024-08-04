// Perlin.h
#ifndef PERLIN_H
#define PERLIN_H

#include <glm/glm.hpp>
#include <cmath>
#include <array>
#include <algorithm>
#include <numeric>
#include <random>

class Perlin {
public:
    Perlin(unsigned int seed = 0);
    float noise(float x, float y) const;
    float noise(float x, float y, float z) const;

private:
    std::array<int, 512> p;
    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y) const;
    float grad(int hash, float x, float y, float z) const;
};

#endif // PERLIN_H
