#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in float FogDepth;

uniform sampler2D texture1;
uniform vec3 fogColor;
uniform float fogDensity;

void main()
{
    vec3 lightDir = normalize(vec3(0.8, 1.0, 0.9));
    float diff = max(dot(Normal, lightDir), 0.2);

    vec4 texColor = texture(texture1, TexCoord);

    float fogFactor = 1.0 - exp(-fogDensity * FogDepth);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Adjust the fog factor to apply fog to distant objects
    fogFactor = 1.0 - fogFactor;

    // Mix the original color with the fog color
    vec4 color = mix(vec4(fogColor, 1.0), vec4(texColor.rgb * diff, texColor.a), fogFactor);

    FragColor = color;
}
