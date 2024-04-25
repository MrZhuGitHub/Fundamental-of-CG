#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix;

uniform float width;

void main()
{
    vec3 direction = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
    vec3 normal = cross(direction, vec3(0, 1, 0));

    vec3 vectex1 = gl_in[0].gl_Position.xyz - normal*width;
    gl_Position = projectionMatrix *viewMatrix * modelMatrix * objPosMatrix * vec4(vectex1, 1.0);
    EmitVertex();

    vec3 vectex2 = gl_in[0].gl_Position.xyz + normal*width;
    gl_Position = projectionMatrix *viewMatrix * modelMatrix * objPosMatrix * vec4(vectex2, 1.0);
    EmitVertex();

    vec3 vectex3 = gl_in[1].gl_Position.xyz - normal*width;
    gl_Position = projectionMatrix *viewMatrix * modelMatrix * objPosMatrix * vec4(vectex3, 1.0);
    EmitVertex();

    vec3 vectex4 = gl_in[1].gl_Position.xyz + normal*width;
    gl_Position = projectionMatrix *viewMatrix * modelMatrix * objPosMatrix * vec4(vectex4, 1.0);
    EmitVertex();

    EndPrimitive();
}