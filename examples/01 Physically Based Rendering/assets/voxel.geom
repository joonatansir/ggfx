#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in gl_PerVertex { vec4 gl_Position; } gl_in[];
out gl_PerVertex { vec4 gl_Position; };

in VertexData
{
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
    vec3 n = normalize(cross(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz, gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz));
    float area = abs(dot(axes[i], n));
    if(area > greatestArea)
    {
      greatestArea = area;
      projectionIndex = i;
    }
  }
  
  return projectionIndex;
}

const mat3 projectionMatrices[3] = mat3[3](mat3(0.0, 0.0, 1.0,
                                                0.0, 1.0, 0.0,
                                                1.0, 0.0, 0.0),
                                           mat3(1.0, 0.0, 0.0,
                                                0.0, 0.0, 1.0,
                                                0.0, 1.0, 0.0),
                                           mat3(1.0, 0.0, 0.0,
                                                0.0, 1.0, 0.0,
                                                0.0, 0.0, 1.0));

void main()
{  
  uint projIdx = getDominantAxis();
  
	for(int i = 0; i < gs_in.length(); i++)
	{
    vec3 positionWithZFlipped = vec3(gl_in[i].gl_Position.xy, gl_in[i].gl_Position.z);
		gs_out.position = (positionWithZFlipped + vec3(1.0, 1.0, 1.0)) / 2.0;
		gs_out.normal = gs_in[i].normal;
		gs_out.textureCoord = gs_in[i].textureCoord;
    
    gl_Position = vec4(projectionMatrices[projIdx] * gl_in[i].gl_Position.xyz, 1.0);
    //gl_Position = gl_in[i].gl_Position;
    
		EmitVertex();
	}
	EndPrimitive();
}