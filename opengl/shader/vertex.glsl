#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0); 
}