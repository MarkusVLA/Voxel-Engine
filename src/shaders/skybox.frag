#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

void main()
{
    vec3 topColor = vec3(0.1, 0.5, 0.8);     
    vec3 horizonColor = vec3(0.529, 0.808, 0.922);
    vec3 bottomColor = vec3(0.2, 0.2, 0.2);
    float t = (TexCoords.y + 1.0) * 0.5;
    vec3 color;
    if (t < 0.5) {
        color = mix(bottomColor, horizonColor, t * 2.0);
    } else {
        color = mix(horizonColor, topColor, (t - 0.5) * 2.0);
    }

    FragColor = vec4(color, 1.0);
}
