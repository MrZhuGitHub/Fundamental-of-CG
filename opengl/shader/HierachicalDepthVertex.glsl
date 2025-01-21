#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;

void main()
{
    gl_Position = vec4(vPosition.x, vPosition.y, 0.0, 1.0);
}