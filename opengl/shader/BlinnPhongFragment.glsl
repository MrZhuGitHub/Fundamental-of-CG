#version 330 core
in vec4 normal;
in vec3 halfLightView;
in vec3 viewDirection;
in vec3 lightDirection;
in vec4 vertexPosition;
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
uniform bool shadowMap;
uniform bool shadow_enable;
uniform sampler2D shadowTexture;
in vec4 CoordInLightCamera;
void main()
{
    vec3 n = normalize(normal.xyz);
    vec3 l = normalize(lightDirection);
    vec3 h = normalize(halfLightView);
    vec3 v = normalize(viewDirection);

    vec3 shadow = ka*Ia;

    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    if ((dot(v, n)*dot(l, n)) >= 0) {
        if (dot(l, n) >= 0.0) {
            diffuse = kd*intensity*dot(l, n);
        } else {
            diffuse = -kd*intensity*dot(l, n);
        }
    }

    vec3 phong = vec3(0.0, 0.0, 0.0); 
    if ((dot(v, n)*dot(l, n)) >= 0) {
        if (dot(h, n) >= 0.0) { 
            phong = ks*intensity*pow(dot(h, n), phongExp);
        } else {
            phong = ks*intensity*pow(-dot(h, n), phongExp);
        }
    }

    vec3 color = shadow + diffuse + phong;
    if (texture_enable > 0) {
        fragmentColor = texture(texture_diffuse1, TexCoord) * vec4(color, 1.0);
    } else {
        fragmentColor = vec4(color, 1.0);
    }

    if (shadow_enable) {
        if (shadowMap) {
            float zDepth = 0.5*vertexPosition.z/vertexPosition.w + 0.5f;
            fragmentColor = vec4(zDepth, zDepth, zDepth, 1.0);
        } else {
            float z =  0.5*CoordInLightCamera.z/CoordInLightCamera.w + 0.5f;
            float x = 0.5*CoordInLightCamera.x/CoordInLightCamera.w + 0.5f;
            float y = 0.5*CoordInLightCamera.y/CoordInLightCamera.w + 0.5f;
            float zMinDepth = texture(shadowTexture, vec2(x, y)).r;

            // Percentage Closer Filter
            float bias = max(0.005 * (1.0 - dot(n, l)), 0.004);

            float shadowPercentage = 25.0;
            vec2 textureUnitSize = 1.0/textureSize(shadowTexture, 0);
            for (float sampleX = -2; sampleX <=2.0; sampleX++) {
                for (float sampleY= -2; sampleY <= 2.0; sampleY++) {
                    float s = texture(shadowTexture, vec2(x, y) + vec2(sampleX, sampleY)*textureUnitSize).r;
                    if (z > (s + bias)) {
                        shadowPercentage--;
                    }
                }
            }
            shadowPercentage = shadowPercentage/25.0;
            fragmentColor = fragmentColor * vec4(shadowPercentage, shadowPercentage, shadowPercentage, 1.0);
        }
    }
}