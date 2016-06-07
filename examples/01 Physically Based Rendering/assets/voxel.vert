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
  vec3 normal;
  vec2 textureCoord;
} vs_out;

void main()
{
  vec4 worldSpaceNormal = model * vec4(inNormal, 1.0);
  vs_out.normal = worldSpaceNormal.xyz;
  vs_out.textureCoord = inTexCoord;
  gl_Position = projection * view * model * vec4(inPosition , 1.0);
}