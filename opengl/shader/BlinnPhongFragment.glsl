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
            // PCSS
            float z =  0.5*CoordInLightCamera.z/CoordInLightCamera.w + 0.5f;
            float x = 0.5*CoordInLightCamera.x/CoordInLightCamera.w + 0.5f;
            float y = 0.5*CoordInLightCamera.y/CoordInLightCamera.w + 0.5f;
            vec2 textureUnitSize = 1.0/textureSize(shadowTexture, 0);

            //bias size
            float shadowMapResolution = max(textureUnitSize.x, textureUnitSize.y);
            float lv = 1.0/sqrt(3.0);
            float bias = 10.0*abs((shadowMapResolution*sqrt(1.0 - dot(n, vec3(-lv, lv, -lv))*dot(n, vec3(-lv, lv, -lv))))/(abs(dot(n, vec3(-lv, lv, -lv)))*2.0));
            bias = max(bias, 0.003);
            bias = min(bias, 0.0045);
            //bias = 0.003;

            //PCS size
            float count = 0.0;
            float sampleSize = 0.0;
            for (float sampleX = -10; sampleX <= 10; sampleX++) {
                for (float sampleY= -10; sampleY <= 10; sampleY++) {
                    float s = texture(shadowTexture, vec2(x, y) + vec2(sampleX, sampleY)*textureUnitSize).r;
                    float index = max(1.0, sqrt(sampleX*sampleX+sampleY*sampleY));
                    if (z > (s + index*bias)) {
                        count++;
                        sampleSize = sampleSize + 100.0*(z - s)/s;
                    }
                }
            }

            float actualSampleSize = 10.0;
            if (count > 0.0) {
                sampleSize = sampleSize/count;
                sampleSize = min(sampleSize, 10.0);
                sampleSize = max(sampleSize, 1.0);
                actualSampleSize = floor(sampleSize);
            } else {
                actualSampleSize = 1.0;
            }

            //compute shadow
            float shadowPercentage = (2*actualSampleSize+1)*(2*actualSampleSize+1);
            float isvalid = 0.0;
            for (float sampleX = -actualSampleSize; sampleX <= actualSampleSize; sampleX++) {
                for (float sampleY= -actualSampleSize; sampleY <= actualSampleSize; sampleY++) {
                    float s = texture(shadowTexture, vec2(x, y) + vec2(sampleX, sampleY)*textureUnitSize).r;
                    float index = max(1.0, sqrt(sampleX*sampleX+sampleY*sampleY));
                    if (sqrt(sampleX*sampleX+sampleY*sampleY) <= actualSampleSize) {
                        isvalid++;
                        if (z > (s + index*bias)) {
                            shadowPercentage--;
                        }
                    } else {
                        shadowPercentage--;
                    }
                }
            }
            shadowPercentage = shadowPercentage/isvalid;
            fragmentColor = fragmentColor * vec4(shadowPercentage*0.7 + 0.3, shadowPercentage*0.7 + 0.3, shadowPercentage*0.7 + 0.3, 1.0);
        }
    }
}