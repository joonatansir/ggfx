#version 450 core

in vec4 voxelColor;
out vec4 color;

void main()
{
  if(voxelColor.a < 0.001)
    discard;
  
  //color = vec4(vec3(voxelColor / (voxelColor.a * 255.0)), 1.0);
  vec4 v = voxelColor;
  //v.a = 0.2;
  color = v;
}