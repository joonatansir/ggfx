#version 450 core

layout (location = 0) in vec3 inPosition;

layout (location = 7) uniform mat4 model;
layout (location = 6) uniform mat4 view;
layout (location = 5) uniform mat4 projection;
layout (location = 8) uniform int gridSize;
layout (location = 9) uniform int gridResolution;

out gl_PerVertex { vec4 gl_Position; };
 
//layout (location = 0) uniform image3D voxelTexture;

void main()
{
  float scale = float(gridSize) / gridResolution;
  float gridStartPosition = gridSize/2.0;
  float halfScale = scale / 2.0;
  float x = -gridStartPosition + halfScale + scale * (gl_InstanceID % gridResolution);
  float y = gridStartPosition - halfScale - scale * ((gl_InstanceID % (gridResolution*gridResolution)) / gridResolution);
  float z = gridStartPosition - halfScale - scale * (gl_InstanceID / (gridResolution*gridResolution));
  vec4 pos = vec4(x, y, z, 1.0);
  mat4 m = model;
  m[3] = pos;
  gl_Position = projection * view * m * vec4(inPosition*scale , 1.0);
}

