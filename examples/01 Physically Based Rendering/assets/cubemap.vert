#version 450 core

out gl_PerVertex
{ 
    vec4 gl_Position;
};

out vec3 coords;

layout (location = 5) uniform mat4 projection;
layout (location = 6) uniform mat4 view;

void main()
{
    vec3[4] vertices = vec3[4] 
    (
        vec3(-1.0, -1.0, 1.0),
        vec3(1.0, -1.0, 1.0),
        vec3(-1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0)
    );
	
    coords = transpose(mat3(view)) * (inverse(projection) * vec4(vertices[gl_VertexID], 1.0)).xyz;

    gl_Position = vec4(vertices[gl_VertexID], 1.0);
}