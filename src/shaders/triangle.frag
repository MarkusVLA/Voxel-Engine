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

void main() {
    // Sample the texture
    vec4 texColor = texture(texture1, TexCoord);
    
    // Alpha testing - discard fragments with low alpha
    if(texColor.a < 0.2)
        discard;

    // Normalize inputs
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightDir);

    // Lambert diffuse lighting
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Combine lighting (ambient + diffuse)
    vec3 lighting = ambient + diffuse;

    // Apply lighting to texture color
    vec3 result = lighting * texColor.rgb;

    // Apply fog
    float fogFactor = 1.0 - exp(-fogDensity * FogDepth);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    fogFactor = 1.0 - fogFactor;
    vec3 color = mix(fogColor, result, fogFactor);

    // Output final color with original alpha
    FragColor = vec4(color, texColor.a);
}