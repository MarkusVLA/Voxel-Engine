#ifndef PERLIN_H
#define PERLIN_H

#include <glm/glm.hpp>
#include <cmath>

class Perlin {
public:
    Perlin();

    float noise(float x, float y) const;

private:
    static const int permutation[256];
    int p[512];

    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y) const;
};



#endif // PERLIN_H
