#version 120

// Input vertex data
attribute vec2 position;
attribute vec2 texCoord;

// Output to fragment shader
varying vec2 v_texCoord;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    v_texCoord = texCoord;
}
