#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture1;

void main()
{
    vec3 lightDir = normalize(vec3(0.8, 1.0, 0.9));
    float diff = max(dot(Normal, lightDir), 0.2);

    vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(texColor.rgb * diff, texColor.a);
}
