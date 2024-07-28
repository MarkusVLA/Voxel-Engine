#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <glad/glad.h>
#include <string>

class TextureLoader {
public:
    static unsigned int loadTexture(const std::string& path);
};

#endif
