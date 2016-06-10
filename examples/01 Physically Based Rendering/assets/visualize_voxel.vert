#version 450 core

layout (location = 0) in vec3 inPosition;

layout (location = 7) uniform mat4 model;
layout (location = 6) uniform mat4 view;
layout (location = 5) uniform mat4 projection;
layout (location = 8) uniform int gridSize;
layout (location = 9) uniform int gridResolution;

layout (binding = 0, rgba32f) coherent uniform image3D voxelTexture;

out gl_PerVertex { vec4 gl_Position; };

out vec4 voxelColor;

void main()
{
  ivec3 c = ivec3(gl_InstanceID % gridResolution, 
                (gl_InstanceID % (gridResolution*gridResolution)) / gridResolution,
                gl_InstanceID / (gridResolution*gridResolution));
  
  voxelColor = imageLoad(voxelTexture, c);

  float scale = float(gridSize) / gridResolution;
  vec3 startPos = (vec3(-gridSize, -gridSize, gridSize) + vec3(scale, scale, -scale)) / 2.0;
  vec3 cubePos = startPos + vec3(c.x, c.y, -c.z) * scale;

  mat4 translatedModel = model;
  translatedModel[3] = vec4(cubePos, 1.0);
  gl_Position = projection * view * translatedModel * vec4(inPosition*scale, 1.0);
}

