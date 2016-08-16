#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 15) uniform mat4 projMatrices[3];
layout (location = 11) uniform int projAxis;

out gl_PerVertex { vec4 gl_Position; };

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 textureCoord;
} gs_in[];

out VoxelData
{
  vec3 position;
  vec3 normal;
  vec2 textureCoord;
  int faceIndex;
} gs_out;

int getDominantAxis()
{
  const vec3 axes[2] = vec3[2](vec3(0.0, 0.0, 1.0),
                               vec3(0.0, 0.0, -1.0));
  
  int projectionIndex;
  float greatestArea = 0.0;
  int axis = projAxis*2;
  for(int i = 0; i < 2; ++i)
  {
    float area = dot(axes[i], normalize(cross(gs_in[1].position - gs_in[0].position, 
                                              gs_in[2].position - gs_in[0].position)));
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
  gs_out.faceIndex = getDominantAxis() + (2 * projAxis);
  
  for(int i = 0; i < 3; i++)
  {
    // convert position from NDC to the voxel image coordinates 
    vec3 c = ((gs_in[i].position + vec3(1.0, 1.0, 1.0)) / 2.0);
    gs_out.position = (projMatrices[projAxis] * vec4(c, 1.0)).xyz;
    gs_out.normal = gs_in[i].normal;
    gs_out.textureCoord = gs_in[i].textureCoord;
    
    gl_Position = vec4(gs_in[i].position, 1.0);
    
    EmitVertex();
  }
  EndPrimitive();
}