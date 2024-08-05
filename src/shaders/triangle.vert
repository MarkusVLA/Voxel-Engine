#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in float aVoxelType;
layout(location = 4) in float aAO;

out vec2 TexCoord;
out vec3 Normal;
out float FogDepth;
out float VoxelType;
out float AO;

uniform mat4 view;
uniform mat4 projection;

const float WATER = 223.0;
const float CACTUS = 70.0;
const float WATER_Y_OFFSET = -0.2;
const float CACTUS_INWARD_SLIDE = 0.1;

void main()
{
    vec3 position = aPos;

    // Add y offset for water voxels
    if (aVoxelType == WATER) {
        position.y += WATER_Y_OFFSET;
    }

    // Add inward slide for cactus faces
    if (aVoxelType == CACTUS) {
        position -= aNormal * CACTUS_INWARD_SLIDE;
    }

    vec4 viewPos = view * vec4(position, 1.0);
    gl_Position = projection * viewPos;

    FogDepth = length(viewPos.xyz);
    VoxelType = aVoxelType;
    AO = aAO;

    float texSize = 1.0 / 16.0;
    vec2 baseCoord = vec2(mod(aVoxelType, 16.0), floor(aVoxelType / 16.0)) * texSize;
    vec2 flippedTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    TexCoord = baseCoord + flippedTexCoord * texSize;
    Normal = aNormal;
}
