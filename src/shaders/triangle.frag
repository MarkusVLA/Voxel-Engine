#version 330 core

const int GRASS = 0;
const int STONE = 1;
const int DIRT = 2;
const int BRICK = 7;
const int COBBLESTONE = 16;
const int SAND = 18;
const int GRAVEL = 19;
const int LOG = 20;
const int IRONORE = 33;
const int TALLGRASS = 39;
const int LEAVES = 52;
const int SNOW = 66;
const int ICE = 67;
const int CACTUS = 70;
const int SANDSTONE = 176;
const int WATER = 223;

out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in float FogDepth;

uniform sampler2D texture1;
uniform vec3 fogColor;
uniform float fogDensity;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform int aVoxelType; 

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    if (texColor.a == 0.0)
        discard;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);

    vec3 diffuse = diff * lightColor;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lighting = ambient + diffuse;
    vec3 result = lighting * texColor.rgb;


    switch (aVoxelType) {
        case GRASS:
            break;

        case STONE:
            break;

        case DIRT:

            break;

        default:
            break;
    }

    float fogFactor = 1.0 - exp(-fogDensity * FogDepth);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    fogFactor = 1.0 - fogFactor;
    vec3 color = mix(fogColor, result, fogFactor);

    FragColor = vec4(color, texColor.a);
}
