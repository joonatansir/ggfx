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

	color = vec4(gl_FragCoord.y/1000.0, 0.0, 0.0, 0.0);
	
	//color = vec4(0.5*normal+0.5, 1.0);
	//color = vec4(diffuse * contribution + ambient, 1.0);
    //color = texture(cubemapSampler, normal) * texture(sampler2, textureCoord);
    //color = mix(texture(sampler, textureCoord), texture(sampler2, textureCoord), 0.5*sin(time)+0.5);
    //color = vec4(position.y/10.0 + 0.5, normal.r, position.y/20.0 + 0.1, 1.0);
	color = texture(sampler2, vec2(textureCoord.x*(sin(time+textureCoord.y)*4.0), textureCoord.y));
	//color = texture(sampler2, textureCoord);
}