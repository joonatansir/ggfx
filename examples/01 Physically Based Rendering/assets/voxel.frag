#version 450 core

layout (location = 0) uniform image3D voxelTexture;

out vec4 color;

void main()
{
  color = vec4(1.0, 0.0, 0.5, 0.2);
}