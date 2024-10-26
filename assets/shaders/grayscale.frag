uniform sampler2D texture;

void main()
{
    vec4 color = texture2D(texture, gl_TexCoord[0].xy);
    float gray = (color.r + color.g + color.b) / 3.0;
    
    vec3 mixedColor = mix(color.rgb, vec3(gray), 1);
    
    gl_FragColor = vec4(mixedColor, color.a);
}
