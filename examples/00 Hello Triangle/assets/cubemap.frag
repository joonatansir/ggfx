#version 450 core

out vec4 color;

in vec3 coords;

uniform samplerCube cubemap;

void main()
{
    color = texture(cubemap, coords);
	//color = 0.5*vec4(coords.x, coords.y, coords.z, 1.0)+0.5;
}