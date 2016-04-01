#version 450 core

out gl_PerVertex
{ 
    vec4 gl_Position;
};

out vec3 coords;

uniform mat4 view;

void main()
{
    vec3[4] vertices = vec3[4] 
    (
        vec3(-1.0, -1.0, 1.0),
        vec3(1.0, -1.0, 1.0),
        vec3(-1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0)
    );

    coords = mat3(view) * vertices[gl_VertexID];

    gl_Position = vec4(vertices[gl_VertexID], 1.0);
}