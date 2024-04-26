#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];
uniform vec3 lightPosition;
out vec4 normal;
out vec3 halfLightView;
out vec3 lightDirection;
out vec2 TexCoord;
void main()
{
    vec4 vVerticPositionInCameraCoordinate = viewMatrix * modelMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0);
    vec4 vNormalInCameraCoordinate = viewMatrix * (vec4(vNormal, 0.0) + modelMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0)) - vVerticPositionInCameraCoordinate;
    normal = vNormalInCameraCoordinate;
    vec4 vLightPositionInCameraCoordinate = viewMatrix * vec4(lightPosition, 1.0);
    lightDirection = normalize(vLightPositionInCameraCoordinate.xyz - vVerticPositionInCameraCoordinate.xyz);
    vec3 viewDirection = normalize(-vVerticPositionInCameraCoordinate.xyz); 
    halfLightView = normalize(viewDirection + lightDirection); 
    TexCoord = aTexCoord;
    gl_Position = projectionMatrix * vVerticPositionInCameraCoordinate; 
}