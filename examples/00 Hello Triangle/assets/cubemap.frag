#version 450 core

out vec4 color;

in VS_OUT
{
    vec3 coords;
} fs_in;

uniform samplerCube cubemap;

void main()
{
    color = texture(cubemap, fs_in.coords);
    //color = vec4(gl_FragCoord.x / 1600.0, gl_FragCoord.y / 900.0, 0.5, 1.0);
}