#version 450 core

layout (binding = 0, rgba32f) uniform image3D voxelTexture;
layout (binding = 2) uniform sampler2D sampler;

in VS_OUT
{
	vec3 normal;
	vec2 textureCoord;
} fs_in;

out vec4 color;

void main()
{
  ivec3 coords = ivec3(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z);
  imageStore(voxelTexture, coords, texture(sampler, fs_in.textureCoord));
  color = vec4(0.5, 0.0, 0.5, 0.5);
}