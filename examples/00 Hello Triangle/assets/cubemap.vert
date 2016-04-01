#version 450 core

out gl_PerVertex
{ 
    vec4 gl_Position;
};

out VS_OUT
{
    vec3 coords;
} vs_out;

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

    vs_out.coords = mat3(view) * vertices[gl_VertexID];

    gl_Position = vec4(vertices[gl_VertexID], 1.0);
}