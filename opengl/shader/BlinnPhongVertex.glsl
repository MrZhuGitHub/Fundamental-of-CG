precision highp float; 
precision highp int; 

attribute vec4 vPosition; 
attribute vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;
uniform vec3 lightPosition;

varying vec4 normal;
varying vec3 halfLightView;
varying vec3 lightDirection;

uniform float x;
uniform float y;
uniform float z;
uniform float homogeneous;

void main()
{
    vec4 vVerticPositionInCameraCoordinate = viewMatrix * modelMatrix * vPosition;
    // vec4 vNormalInCameraCoordinate = normalMatrix * vec4(vNormal, 0.0);
    vec4 vNormalInCameraCoordinate = viewMatrix * (vec4(vNormal, 0.0) + modelMatrix * vPosition) - vVerticPositionInCameraCoordinate;
    normal = vNormalInCameraCoordinate;
    vec4 vLightPositionInCameraCoordinate = viewMatrix * vec4(lightPosition, 1.0);
    lightDirection = normalize(vLightPositionInCameraCoordinate.xyz - vVerticPositionInCameraCoordinate.xyz);
    vec3 viewDirection = normalize(-vVerticPositionInCameraCoordinate.xyz); 
    halfLightView = normalize(viewDirection + lightDirection); 
    gl_Position = projectionMatrix * vVerticPositionInCameraCoordinate; 
}