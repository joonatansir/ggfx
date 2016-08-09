#version 450 core

layout (r32ui, binding = 0) uniform uimage3D voxelImage;
layout (binding = 2) uniform sampler2D sampler;
layout (location = 9) uniform int gridResolution;

in VoxelData
{
  vec3 position;
  vec3 normal;
  vec2 textureCoord;
} fs_in;

out vec4 color;

vec4 convRGBA8ToVec4( uint val)
{
  return vec4 (float((val & 0x000000FF)), 
               float((val & 0x0000FF00) >> 8U), 
               float((val & 0x00FF0000) >> 16U), 
               float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8( vec4 val)
{
  return (uint(val.w) & 0x000000FF) << 24U | 
         (uint(val.z) & 0x000000FF) << 16U | 
         (uint(val.y) & 0x000000FF) << 8U | 
         (uint(val.x) & 0x000000FF);
}

void main()
{
  float res = gridResolution - 1;
  ivec3 coords = ivec3(round(fs_in.position.x*(res)),// + fs_in.faceIndex * gridResolution), 
                       round(fs_in.position.y*(res)), 
                       round((fs_in.position.z)*(res)));
  
  vec4 val = texture(sampler, fs_in.textureCoord);
  val.rgba *= 255.0f;
  uint newVal = convVec4ToRGBA8(val);
  uint prevStoredVal = 0; 
  uint curStoredVal;
    
  while((curStoredVal = imageAtomicCompSwap(voxelImage, coords, prevStoredVal, newVal)) != prevStoredVal)
  {
    prevStoredVal = curStoredVal;
    vec4 rval = convRGBA8ToVec4( curStoredVal);
    vec4 avg = (rval + val) / 2.0;
    newVal = convVec4ToRGBA8(avg);
  }
  
  for(int i = 1; i < 6; i++)
  {
    ivec3 s = coords;
    s.x += i * gridResolution;
    imageAtomicExchange(voxelImage, s, 0xffffffff);
  }
}