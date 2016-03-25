#version 330 core

layout (location = 0) in vec3 inPosition;
//layout (location = 1) in vec2 inTexCoords;

out gl_PerVertex { vec4 gl_Position; };
out vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //texCoords = inTexCoords;
    position = inPosition;
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
}