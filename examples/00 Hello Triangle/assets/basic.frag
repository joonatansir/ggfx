#version 330 core

in vec3 position;
in vec3 normal;
in vec2 textureCoord;

out vec4 color;

uniform float time;
uniform sampler2D sampler;
uniform sampler2D sampler2;

void main()
{
    //color = mix(texture(sampler, textureCoord), texture(sampler2, textureCoord), 0.5*sin(time)+0.5) + vec4(normal, 1.0);
    color = vec4(normal, 1.0);
}