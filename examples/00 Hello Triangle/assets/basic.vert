#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out gl_PerVertex { vec4 gl_Position; };
out vec3 position;
out vec3 normal;
out vec2 textureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

void main()
{
    textureCoord = inTexCoord;
    normal = inNormal;
    position = inPosition;
    
    //position.z -= gl_InstanceIDasd;
    float gap = 3.0;
    position += vec3(mod(gl_InstanceID, 10) * gap, 0.0, -trunc(gl_InstanceID / 10) * gap);

    gl_Position = projection * view * model * vec4(position, 1.0);
    
}