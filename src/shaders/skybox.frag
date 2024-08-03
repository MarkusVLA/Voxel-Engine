#version 330 core
out vec4 FragColor;
in vec3 TexCoords;
uniform float time;

// Improved hash function
vec3 hash3(vec3 p) {
    p = vec3(dot(p,vec3(127.1,311.7,74.7)),
             dot(p,vec3(269.5,183.3,246.1)),
             dot(p,vec3(113.5,271.9,124.6)));
    return -1.0 + 2.0 * fract(sin(p)*43758.5453123);
}

// Improved noise function
float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(dot(hash3(i + vec3(0.0,0.0,0.0)), f - vec3(0.0,0.0,0.0)),
                       dot(hash3(i + vec3(1.0,0.0,0.0)), f - vec3(1.0,0.0,0.0)), u.x),
                   mix(dot(hash3(i + vec3(0.0,1.0,0.0)), f - vec3(0.0,1.0,0.0)),
                       dot(hash3(i + vec3(1.0,1.0,0.0)), f - vec3(1.0,1.0,0.0)), u.x), u.y),
               mix(mix(dot(hash3(i + vec3(0.0,0.0,1.0)), f - vec3(0.0,0.0,1.0)),
                       dot(hash3(i + vec3(1.0,0.0,1.0)), f - vec3(1.0,0.0,1.0)), u.x),
                   mix(dot(hash3(i + vec3(0.0,1.0,1.0)), f - vec3(0.0,1.0,1.0)),
                       dot(hash3(i + vec3(1.0,1.0,1.0)), f - vec3(1.0,1.0,1.0)), u.x), u.y), u.z);
}

// Fractal Brownian Motion (fBm) for more detailed clouds
float fbm(vec3 p) {
    float f = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < 4; i++) {
        f += amplitude * noise(frequency * p);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return f;
}

void main()
{
    // Define sky colors
    vec3 skyBlue = vec3(0.4, 0.7, 1.0);
    vec3 horizonColor = vec3(0.7, 0.8, 1.0);
    
    // Calculate sky gradient
    float t = max(TexCoords.y, 0.0);
    vec3 skyColor = mix(horizonColor, skyBlue, pow(t, 0.5));
    
    // Generate clouds
    vec3 cloudCoord = TexCoords * 3.0 + vec3(time * 0.05, 0.0, time * 0.025);
    float cloudNoise = fbm(cloudCoord);
    
    // Shape the clouds with sharper edges
    float cloudShape = smoothstep(0.5, 0.55, cloudNoise);
    cloudShape = pow(cloudShape, 1.5); // Increase contrast
    
    // Add some variation to cloud density
    cloudShape *= (0.8 + 0.2 * noise(cloudCoord * 4.0 + vec3(time * 0.1)));
    
    // Create final cloud color
    vec3 cloudColor = vec3(1.0); // Pure white clouds
    
    // Mix clouds with sky
    vec3 finalColor = mix(skyColor, cloudColor, cloudShape);
    
    FragColor = vec4(finalColor, 1.0);
}