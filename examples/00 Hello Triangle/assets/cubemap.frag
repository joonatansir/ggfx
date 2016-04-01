#version 450 core

out vec4 color;

in vec3 coords;

uniform samplerCube cubemap;

void main()
{
    color = texture(cubemap, coords);
}