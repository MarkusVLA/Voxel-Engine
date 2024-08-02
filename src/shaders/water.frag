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
    // Normalize inputs
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightDir);

    // Blinn-Phong specular highlights
    vec3 viewDir = vec3(0.0, 0.0, 1.0); // assuming view direction is along z-axis
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0); // shininess factor
    vec3 specular = spec * lightColor;

    // Lambertian diffuse lighting
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Combine lighting
    vec3 lighting = ambient + diffuse + specular;

    // Texture color
    vec4 texColor = texture(texture1, TexCoord);
    vec3 result = lighting * texColor.rgb;

    // Apply fog
    float fogFactor = 1.0 - exp(-fogDensity * FogDepth);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    fogFactor = 1.0 - fogFactor;
    vec4 color = mix(vec4(fogColor, 1.0), vec4(result, texColor.a), fogFactor);

    // Add blue tint and transparency
    vec3 waterColor = vec3(0.0, 0.3, 0.6);
    vec4 waterColorVec = vec4(waterColor, 0.6);  // Transparency
    FragColor = mix(color, waterColorVec, 0.5);
}
