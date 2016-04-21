#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out gl_PerVertex { vec4 gl_Position; };

out VS_OUT
{
	vec3 normal;
	vec2 textureCoord;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

const float pi = 3.141592;

void main()
{
    vs_out.normal = (model * vec4(inNormal, 1.0)).xyz;
	vs_out.textureCoord = inTexCoord;
	
    gl_Position = projection * view * model * vec4(inPosition , 1.0);
}