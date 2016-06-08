#version 450 core

in vec4 voxelColor;
out vec4 color;

void main()
{
  color = voxelColor;
  gl_FragDepth = 1.0 - voxelColor.a;
}