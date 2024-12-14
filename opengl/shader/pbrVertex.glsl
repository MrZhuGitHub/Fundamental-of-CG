#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];
uniform vec3 lightPosition;
uniform vec3 camearPosition;
out vec4 normal;
out vec3 halfLightView;
out vec3 lightDirection;
out vec2 TexCoord;
out vec3 viewDirection;
out vec4 vertexPosition;
uniform mat4 shadowMatrix;
out vec4 CoordInLightCamera;

void main()
{
    vec4 vVerticPositionInModelCoordinate = modelMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0);
    normal = normalize(modelMatrix * objPosMatrix[gl_InstanceID] * (vec4(vNormal, 0.0) + vec4(vPosition, 1.0)) - vVerticPositionInModelCoordinate);
    lightDirection = normalize(lightPosition.xyz - vVerticPositionInModelCoordinate.xyz);
    viewDirection = normalize(camearPosition - vVerticPositionInModelCoordinate.xyz);
    halfLightView = normalize(viewDirection + lightDirection); 
    TexCoord = aTexCoord;
    vertexPosition = projectionMatrix * viewMatrix * vVerticPositionInModelCoordinate;
    gl_Position = projectionMatrix * viewMatrix * vVerticPositionInModelCoordinate;
    CoordInLightCamera = shadowMatrix * vVerticPositionInModelCoordinate;
}
