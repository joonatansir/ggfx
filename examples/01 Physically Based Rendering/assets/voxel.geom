#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 15) uniform mat3 projectionMatrices[3];

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
} gs_out;

uint getDominantAxis()
{
  const vec3 axes[3] = vec3[3](vec3(1.0, 0.0, 0.0),
                               vec3(0.0, 1.0, 0.0),
                               vec3(0.0, 0.0, 1.0));
  
  uint projectionIndex;
  float greatestArea = 0.0;
  for(int i = 0; i < 3; ++i)
  {
    float area = abs(dot(axes[i], normalize(cross(gs_in[0].position - gs_in[1].position, gs_in[0].position - gs_in[2].position))));
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
  uint projectionIndex = getDominantAxis();
  
	for(int i = 0; i < 3; i++)
	{
		gs_out.position = (gs_in[i].position + vec3(1.0, 1.0, 1.0)) / 2.0;
		gs_out.normal = gs_in[i].normal;
		gs_out.textureCoord = gs_in[i].textureCoord;
    
    gl_Position = vec4(projectionMatrices[projectionIndex] * gs_in[i].position, 1.0);
    
		EmitVertex();
	}
	EndPrimitive();
}