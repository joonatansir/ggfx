#version 450 core

layout (location = 0) in vec3 inPosition;

layout (location = 7) uniform mat4 model;
layout (location = 6) uniform mat4 view;
layout (location = 5) uniform mat4 projection;
layout (location = 8) uniform int gridSize;
layout (location = 9) uniform int gridResolution;

layout (binding = 0, rgba32f) uniform image3D voxelTexture;

out gl_PerVertex { vec4 gl_Position; };

out vec4 voxelColor;

void main()
{
  vec3 c = vec3(gl_InstanceID % gridResolution, 
                (gl_InstanceID % (gridResolution*gridResolution)) / gridResolution,
                gl_InstanceID / (gridResolution*gridResolution));
  voxelColor = imageLoad(voxelTexture, vec3(c));

  float scale = float(gridSize) / gridResolution;
  float gridStartPosition = gridSize / 2.0 - scale / 2.0;
  float x = -gridStartPosition + scale * c.x;
  float y = -gridStartPosition + scale * c.y;
  float z = gridStartPosition - scale * c.z;
  vec4 pos = vec4(x, y, z, 1.0);
  mat4 m = model;
  m[3] = pos;
  gl_Position = projection * view * m * vec4(inPosition*scale, 1.0);
}

