#version 450 core

layout (binding = 0, rgba32f) coherent uniform image3D voxelTexture;
layout (binding = 1) uniform sampler2D sampler;
layout (location = 9) uniform int gridResolution;

in VoxelData
{
  vec3 position;
	vec3 normal;
	vec2 textureCoord;
} fs_in;

out vec4 color;

void main()
{
  float res = gridResolution - 1;
  ivec3 coords = ivec3(round(fs_in.position.x*(res)), 
                       round(fs_in.position.y*(res)), 
                       round(fs_in.position.z*(res)));
  imageStore(voxelTexture, coords, texture(sampler, fs_in.textureCoord));
  color = vec4(0.5, 0.0, 0.5, 0.5);
}