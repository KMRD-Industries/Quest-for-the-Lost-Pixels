#version 120

varying vec2 v_texCoord;// Passed from the vertex shader

// Uniforms passed from C++ code
uniform vec2 lightPosition;// Position of the light source
uniform float lightRadius;// Light radius

uniform sampler2D texture;// The texture of the sprite

void main() {
    vec4 texColor = texture2D(texture, v_texCoord);// Get the texture color

    // Calculate the distance from the current fragment to the light source
    float dist = distance(gl_FragCoord.xy, lightPosition);

    // Light intensity fades with distance
    float intensity = smoothstep(0.0, lightRadius, dist);

    // Combine the texture color with the lighting effect
    vec4 lightColor = vec4(1.0, 1.0, 1.0, intensity);// White light with intensity fade
    gl_FragColor = texColor * lightColor;
}
