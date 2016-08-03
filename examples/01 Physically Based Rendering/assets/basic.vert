#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

layout (location = 5) uniform mat4 projection;
layout (location = 6) uniform mat4 view;
layout (location = 7) uniform mat4 model;

out gl_PerVertex { vec4 gl_Position; };

out VS_OUT
{
  vec3 position;
	vec3 normal;
	vec2 textureCoord;
} vs_out;

void main()
{
  vs_out.normal = inNormal;
	vs_out.textureCoord = inTexCoord;
	
  vs_out.position = (model * vec4(inPosition , 1.0)).xyz;
  gl_Position = projection * view * vec4(vs_out.position, 1.0);
}