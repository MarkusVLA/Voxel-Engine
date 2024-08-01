// Perlin.h
#ifndef PERLIN_H
#define PERLIN_H

#include <glm/glm.hpp>
#include <cmath>

class Perlin {
public:
    Perlin();
    float noise(float x, float y) const;
    float noise(float x, float y, float z) const;

private:
    static const int permutation[256];
    int p[512];
    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y) const;
    float grad(int hash, float x, float y, float z) const;
};

#endif // PERLIN_H