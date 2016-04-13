#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out gl_PerVertex { vec4 gl_Position; };

smooth out vec3 position;
smooth out vec3 normal;
smooth out vec2 textureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

const float pi = 3.141592;

void main()
{
    textureCoord = inTexCoord;
    normal = (model * vec4(inNormal, 1.0)).xyz;
	position = inPosition;
    
    //position.z -= gl_InstanceIDasd;
    //float gap = 4.0;
	//float posX = mod(gl_InstanceID, 10) * gap;
	//float posZ = -trunc(gl_InstanceID / 10) * gap;
	//float posY = sin(time*2.5+posX/5.0)*4.0 * sin(time*10.0+posZ/10.0)*2.0;
	//float ring = (trunc(gl_InstanceID / 100)+1);
	//float radius = ring * 10.0;
	//float step = (2.0*pi)/100.0;
	//vec3 dir = vec3(sin(gl_InstanceID*step)*radius, 0.0, cos(gl_InstanceID*step)*radius);
	
	//if(gl_InstanceID < 100)
	//	position += dir*9999;
	//else
	//	position += dir + vec3(0.0, sin(time*4.0+gl_InstanceID)*ring*2.0, 0.0);// + dir * cos(time);
		
	//position.x += sin(time * 10.0 + position.y);
	//position.z += cos(time * 5.0 + position.y * 2.0);
	
    gl_Position = projection * view * model * vec4(position , 1.0);
}