#version 450 core

in vec4 voxelColor;
out vec4 color;

void main()
{
  if(voxelColor.a == 0)
    discard;

  color = voxelColor;
}