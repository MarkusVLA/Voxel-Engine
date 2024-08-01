#version 330 core
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

void main()
{
    // Lambert lighting
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient light
    vec3 ambient = ambientStrength * lightColor;

    // Combine lighting
    vec3 lighting = ambient + diffuse;

    vec4 texColor = texture(texture1, TexCoord);
    vec3 result = lighting * texColor.rgb;

    // Apply fog
    float fogFactor = 1.0 - exp(-fogDensity * FogDepth);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    fogFactor = 1.0 - fogFactor;
    
    vec4 color = mix(vec4(fogColor, 1.0), vec4(result, texColor.a), fogFactor);

    FragColor = color;
}