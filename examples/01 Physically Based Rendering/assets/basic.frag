#version 450 core

in VS_OUT
{
  vec3 normal;
  vec2 textureCoord;
} fs_in;

out vec4 color;

//layout (binding = 1) uniform sampler2D sampler;
layout (binding = 2) uniform sampler2D sampler2;

void main()
{
  color = vec4(texture(sampler2, fs_in.textureCoord));
}