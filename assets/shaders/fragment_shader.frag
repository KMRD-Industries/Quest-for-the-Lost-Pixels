uniform float shadeFactor;  // Uniform to control shading
uniform sampler2D texture;  // The texture sampler

varying vec4 vert_pos;      // Varying variable passed from the vertex shader

void main()
{
    // Sample the texture using the texture coordinates
    vec4 color = texture2D(texture, gl_TexCoord[0].st);

    // Apply shading based on shadeFactor
    color.rgb *= shadeFactor;

    // Output the final color
    gl_FragColor = color * gl_Color;
}
