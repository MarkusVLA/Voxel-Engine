#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in float FogDepth;

uniform sampler2D waterTexture;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float time;
uniform vec3 fogColor;
uniform float fogDensity;

void main()
{
    // Water effect
    vec2 distortedTexCoord = TexCoord;
    distortedTexCoord.x += sin(distortedTexCoord.y * 10.0 + time * 0.1) * 0.01;
    distortedTexCoord.y += cos(distortedTexCoord.x * 10.0 + time * 0.1) * 0.01;
    
    vec4 waterColor = texture(waterTexture, distortedTexCoord);
    
    // Lighting
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = diffuse * waterColor.rgb;
    
    // Apply fog
    float fogFactor = 1.0 - exp(-fogDensity * FogDepth);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    result = mix(result, fogColor, fogFactor);
    
    FragColor = vec4(result, 0.8); // Water transparency
}