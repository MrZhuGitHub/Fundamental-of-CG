#version 330 core
in vec4 normal;
in vec3 halfLightView;
in vec3 lightDirection;
out vec4 fragmentColor;
uniform vec3 intensity;
uniform vec3 Ia;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float phongExp;
in vec2 TexCoord;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform int texture_enable;
void main()
{
    vec3 n = normalize(normal.xyz);
    vec3 l = normalize(lightDirection);
    vec3 h = normalize(halfLightView);
    vec3 shadow = ka*Ia;
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    if (dot(l, n) >= 0.0) {
        diffuse = kd*intensity*dot(l, n);
    }
    vec3 phong = vec3(0.0, 0.0, 0.0); 
    if (dot(h, n) >= 0.0) { 
        phong = ks*intensity*pow(dot(h, n), phongExp);
    } 
    vec3 color = shadow + diffuse + phong;
    if (texture_enable > 0) {
        fragmentColor = texture(texture_diffuse1, TexCoord) * vec4(color, 1.0);
    } else {
        fragmentColor = vec4(color, 1.0);
    } 
}