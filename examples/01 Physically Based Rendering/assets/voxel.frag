#version 450 core

layout (r32ui, binding = 0) uniform uimage3D voxelImage;
layout (binding = 1) uniform sampler2D sampler;
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
 
/*void imageAtomicRGBA8Avg(coherent uimage3D img, ivec3 coords, vec4 val)
{
  val.rgb *= 255.0f; // Optimise following calculations
  uint newVal = convVec4ToRGBA8(val);
  uint prevStoredVal = 0; 
  uint curStoredVal;
    
  // Loop as long as destination value gets changed by other threads
  while((curStoredVal = imageAtomicCompSwap(img, coords, prevStoredVal, newVal)) != prevStoredVal)
  {
    prevStoredVal = curStoredVal;
    vec4 rval = convRGBA8ToVec4( curStoredVal);
    rval.xyz =(rval.xyz * rval.w);                // Denormalize
    vec4 curValF = rval + val;                    // Add new value
    curValF. xyz /= (curValF.w);                  // Renormalize
    newVal = convVec4ToRGBA8( curValF );
  }
}*/

/*void imageAtomicFloatAdd(coherent volatile uimage3D imgUI , ivec3 coords , float val )
{
  uint newVal = floatBitsToUint( val );
  uint prevVal = 0; uint curVal ;
  // Loop as long as destination value gets changed by other threads
  while ( ( curVal = imageAtomicCompSwap(imgUI, coords, prevVal, newVal)) != prevVal )
  {
    prevVal = curVal ;
    newVal = floatBitsToUint(( val + uintBitsToFloat( curVal )));
  }
}*/

void main()
{
  float res = gridResolution - 1;
  ivec3 coords = ivec3(round(fs_in.position.x*(res)), 
                       round(fs_in.position.y*(res)), 
                       round((fs_in.position.z)*(res)));
                       
  //imageAtomicRGBA8Avg(voxelImage, coords, texture(sampler, fs_in.textureCoord));
  //imageStore(voxelImage, coords, texture(sampler, fs_in.textureCoord));
  
  /*vec4 val = texture(sampler, fs_in.textureCoord);
  val.a = 255;
  val.rgb *= 255.0f; // Optimise following calculations
  uint newVal = convVec4ToRGBA8(val);
  uint prevStoredVal = 0; 
  uint curStoredVal;
    
  // Loop as long as destination value gets changed by other threads
  while((curStoredVal = imageAtomicCompSwap(voxelImage, coords, prevStoredVal, newVal)) != prevStoredVal)
  {
    prevStoredVal = curStoredVal;
    vec4 rval = convRGBA8ToVec4( curStoredVal);
    
    rval.xyz *= rval.a;                // Denormalize
    vec4 curValF = rval + val;                    // Add new value
    curValF.xyz /= (curValF.w);                  // Renormalize
    newVal = convVec4ToRGBA8( curValF );
  }*/
  
  vec4 albedo = texture(sampler, fs_in.textureCoord) * 255.0;
  albedo.a = 1;
  uint data = convVec4ToRGBA8(albedo);
  imageAtomicAdd(voxelImage, coords, data);
}