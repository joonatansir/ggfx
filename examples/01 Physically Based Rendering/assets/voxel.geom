#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[];

out gl_PerVertex
{
  vec4 gl_Position;
};

in VS_OUT
{
	vec3 normal;
	vec2 textureCoord;
} gs_in[];

out VS_OUT
{
	vec3 normal;
	vec2 textureCoord;
} gs_out;

void main()
{
	for(int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		gs_out.normal = gs_in[i].normal;
		gs_out.textureCoord = gs_in[i].textureCoord;
		EmitVertex();
	}
	EndPrimitive();
}