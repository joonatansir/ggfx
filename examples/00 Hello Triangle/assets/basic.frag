#version 450 core

smooth in vec3 position;
smooth in vec3 normal;
smooth in vec2 textureCoord;

out vec4 color;

uniform float time;
uniform sampler2D sampler;
uniform sampler2D sampler2;
uniform samplerCube cubemapSampler;

void main()
{
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

    vec3 ambient = vec3(0.0, 0.0, 0.0);
    vec3 diffuse = normal;
    float contribution = max(0.0, dot(lightDir, normal));
    
	//color = vec4(diffuse * contribution + ambient, 1.0);
    color = texture(cubemapSampler, normal) * texture(sampler, textureCoord);
    //color = mix(texture(sampler, textureCoord), texture(sampler2, textureCoord), 0.5*sin(time)+0.5);
    //color = vec4(normal, 1.0);
}