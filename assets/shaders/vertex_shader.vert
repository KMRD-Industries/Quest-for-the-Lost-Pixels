// vertex_shader.vert

varying vec4 vert_pos;  // Declaring a varying variable to pass to the fragment shader

void main()
{
    // Transform the vertex position
    vert_pos = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_Position = vert_pos;

    // Transform the texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // Forward the vertex color
    gl_FrontColor = gl_Color;
}
