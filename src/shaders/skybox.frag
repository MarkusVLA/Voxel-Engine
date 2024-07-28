#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

void main()
{
    // Define colors
    vec3 topColor = vec3(0.1, 0.1, 0.6);      // Less dark blue at the top
    vec3 horizonColor = vec3(0.529, 0.808, 0.922); // Sky blue at the horizon
    vec3 bottomColor = vec3(0.2, 0.2, 0.2);   // Dark grey at the bottom

    // Normalize y coordinate to range [0, 1]
    float t = (TexCoords.y + 1.0) * 0.5;

    vec3 color;
    if (t < 0.5) {
        // Interpolate from bottomColor to horizonColor
        color = mix(bottomColor, horizonColor, t * 2.0);
    } else {
        // Interpolate from horizonColor to topColor
        color = mix(horizonColor, topColor, (t - 0.5) * 2.0);
    }

    FragColor = vec4(color, 1.0);
}
