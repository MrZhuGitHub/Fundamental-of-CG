precision highp float;

varying vec4 normal;
varying vec3 halfLightView;
varying vec3 lightDirection;

uniform vec3 intensity;
uniform vec3 Ia;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float phongExp;

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
    gl_FragColor = vec4(color, 1.0);
}