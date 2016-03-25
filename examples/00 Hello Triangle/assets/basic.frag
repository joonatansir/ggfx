#version 330 core

in vec3 position;

out vec4 color;

uniform float time;
uniform sampler2D sampler;
uniform sampler2D sampler2;

void main()
{
    //color = mix(texture(sampler, texCoords), texture(sampler2, texCoords), 0.5*sin(time)+0.5);
    color = vec4(0.5 * position + 0.5, 1.0);
}