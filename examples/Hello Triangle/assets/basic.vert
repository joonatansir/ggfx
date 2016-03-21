#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoords;

out gl_PerVertex { vec4 gl_Position; };
out vec3 vertexColor;
out vec2 texCoords;

void main()
{
    texCoords = inTexCoords;
    vertexColor = inColor;
    gl_Position = vec4(inPosition, 1.0);
}
