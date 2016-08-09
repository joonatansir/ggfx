#version 450 core

layout (binding = 0, rgba8) uniform image3D voxelTexture;
layout (location = 9) uniform int gridResolution;

in vec4 voxelColor;
in vec3 normal;
flat in ivec3 voxelCoords;

out vec4 color;

int getDominantAxis(vec3 normal)
{
  const vec3 axes[6] = vec3[6](vec3(1.0, 0.0, 0.0),
                               vec3(0.0, 1.0, 0.0),
                               vec3(0.0, 0.0, 1.0),
                               vec3(-1.0, 0.0, 0.0),
                               vec3(0.0, -1.0, 0.0),
                               vec3(0.0, 0.0, -1.0));
  
  int projectionIndex = 0;
  float greatestArea = 0.0;
  for(int i = 0; i < axes.length(); ++i)
  {
    float area = dot(axes[i], normal);
    if(area > greatestArea)
    {
      greatestArea = area;
      projectionIndex = i;
    }
  }
  
  return projectionIndex;
}

void main()
{
  //get the color for the correct face
  ivec3 coords = voxelCoords;
  //coords.x += getDominantAxis(normal) * gridResolution;
  
  vec4 c = imageLoad(voxelTexture, coords); 
  
  if(c.a == 0)
    discard;
  
  color = vec4(normal, 1.0);
}