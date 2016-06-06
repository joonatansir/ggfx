#version 450 core

in vec3 coords;

layout (binding = 3) uniform samplerCube cubemap;

out vec4 color;

void main()
{
    color = texture(cubemap, coords);
}