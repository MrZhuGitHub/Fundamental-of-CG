#version 330 core
precision highp float;
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];
out vec4 vertexPosition;
out vec4 glPosition;
void main()
{
    vec4 vVerticPositionInModelCoordinate = modelMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0);
    vec4 normal = normalize(modelMatrix * objPosMatrix[gl_InstanceID] * (vec4(vNormal, 0.0) + vec4(vPosition, 1.0)) - vVerticPositionInModelCoordinate);
    gl_Position = projectionMatrix * viewMatrix * vVerticPositionInModelCoordinate;
    vertexPosition = vVerticPositionInModelCoordinate;
    glPosition = gl_Position;
}