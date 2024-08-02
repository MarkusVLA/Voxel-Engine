#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aVoxelType;

out vec2 TexCoord;
out vec3 Normal;
out float FogDepth;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPos = view * vec4(aPos, 1.0);
    gl_Position = projection * viewPos;
    
    FogDepth = length(viewPos.xyz);

    float texSize = 1.0 / 16.0;
    vec2 baseCoord = vec2(mod(aVoxelType, 16.0), floor(aVoxelType / 16.0)) * texSize;
    TexCoord = baseCoord + aTexCoord * texSize;

    Normal = aNormal;
}
