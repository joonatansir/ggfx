#version 450 core

in VS_OUT
{
	vec3 normal;
	vec2 textureCoord;
} fs_in;

out vec4 color;

uniform float time;
uniform sampler2D sampler;
uniform sampler2D sampler2;
uniform samplerCube cubemapSampler;

void main()
{
	color = vec4(0.5*fs_in.normal+0.5, 1.0);
}