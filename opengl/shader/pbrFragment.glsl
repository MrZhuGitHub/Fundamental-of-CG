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

uniform samplerCube skybox;
uniform sampler2D EavgTexture;
uniform sampler2D MicroModelBrdfTexture;
uniform float maxMipmapLevel;

uniform vec3 modelColor;
uniform float roughness;

const float PI = 3.1415926;

vec3 averageFresnel(vec3 fresnel) {
    vec3 average = vec3(0.0, 0.0, 0.0);
    for (float j = 1.0; j <= 100; j = j + 1.0) {
        float sinTheta = (2.0*(j - 1.0))/200.0;
        float cosTheta = sqrt(1.0 - sinTheta*sinTheta);
        float k = pow(1.0 - cosTheta, 5.0);
        vec3 fresnelEquation = fresnel + (vec3(1.0, 1.0, 1.0) - fresnel) * vec3(k, k, k);
        average = average + vec3(2.0, 2.0, 2.0)*vec3(0.01, 0.01, 0.01)*vec3(sinTheta, sinTheta, sinTheta)*fresnelEquation;
    }
    return average;
}

void main()
{
    vec3 n = normalize(normal.xyz);
    vec3 l = normalize(lightDirection);
    vec3 h = normalize(halfLightView);
    vec3 v = normalize(viewDirection);

    if (!shadowMap) {
        vec3 reflectLight = 2 * dot(n, v) * n - v;
        float mipmapLevel = maxMipmapLevel * roughness;
        vec4 radiance = textureLod(skybox, reflectLight, mipmapLevel);

        float cosTheta = dot(n, v);
        vec3 fresnel;
        if (texture_enable > 0) {
            fresnel = texture(texture_diffuse1, TexCoord).xyz;
        } else {
            fresnel = modelColor;
        }

        vec3 MicroBrdfVaule = texture(MicroModelBrdfTexture, vec2(roughness, 1.0 - cosTheta)).xyz;
        vec3 EavgValue = texture(EavgTexture, vec2(cosTheta, roughness)).xyz;

        float kullaContyEnergy = MicroBrdfVaule.x + MicroBrdfVaule.y;
        //kullaContyEnergy = pow((1.0 - kullaContyEnergy), 2)/(PI*(1.0 - EavgValue.x));
        vec3 avgFresnel = averageFresnel(fresnel);
        vec3 bounceLoss = avgFresnel*EavgValue/(vec3(1.0, 1.0, 1.0) - avgFresnel*(vec3(1.0, 1.0, 1.0) - EavgValue));

        vec3 MicroBrdf = fresnel*MicroBrdfVaule.x
                       + vec3(1.0, 1.0, 1.0)*MicroBrdfVaule.y;

        //kullaContyEnergy = 1.0;
        //fragmentColor = vec4(radiance.xyz*(MicroBrdf + vec3(1.0 - kullaContyEnergy, 1.0 - kullaContyEnergy, 1.0 - kullaContyEnergy)*bounceLoss), 1.0);
        //fragmentColor = vec4(radiance.xyz*(MicroBrdf + kullaContyEnergy * bounceLoss), 1.0);

        //float addtional = ((cosTheta > 0) ? cosTheta : 0);
        fragmentColor = vec4(radiance.xyz*(MicroBrdf + vec3(1.0 - kullaContyEnergy, 1.0 - kullaContyEnergy, 1.0 - kullaContyEnergy)*bounceLoss), 1.0);
        //fragmentColor = vec4(vec3(1.0, 1.0, 1.0)*(MicroBrdf + vec3(1.0 - kullaContyEnergy, 1.0 - kullaContyEnergy, 1.0 - kullaContyEnergy)*bounceLoss), 1.0);

        //fragmentColor = vec4(vec3(kullaContyEnergy, kullaContyEnergy, kullaContyEnergy), 1.0);

    }

    if (shadow_enable) {
        if (shadowMap) {
            float zDepth = 0.5*vertexPosition.z/vertexPosition.w + 0.5f;
            fragmentColor = vec4(zDepth, zDepth, zDepth, 1.0);
        } else {
            float z =  0.5*CoordInLightCamera.z/CoordInLightCamera.w + 0.5f;
            float x = 0.5*CoordInLightCamera.x/CoordInLightCamera.w + 0.5f;
            float y = 0.5*CoordInLightCamera.y/CoordInLightCamera.w + 0.5f;
            vec2 textureUnitSize = 1.0/textureSize(shadowTexture, 0);

            //bias size
            // float shadowMapResolution = max(textureUnitSize.x, textureUnitSize.y);
            // float lv = 1.0/sqrt(3.0);
            // float bias = 10.0*abs((shadowMapResolution*sqrt(1.0 - dot(n, vec3(-lv, lv, -lv))*dot(n, vec3(-lv, lv, -lv))))/(abs(dot(n, vec3(-lv, lv, -lv)))*2.0));
            // bias = max(bias, 0.0025);
            // bias = min(bias, 0.0045);

            float bias = 0.003;

            // Percentage Closer Filter
            float shadowPercentage = 121.0;
            for (float sampleX = -5; sampleX <=5.0; sampleX++) {
                for (float sampleY= -5; sampleY <= 5.0; sampleY++) {
                    float s = texture(shadowTexture, vec2(x, y) + vec2(sampleX, sampleY)*textureUnitSize).r;
                    float index = max(1.0, sqrt(sampleX*sampleX+sampleY*sampleY));
                    if (z > (s + index*bias)) {
                        shadowPercentage--;
                    }
                }
            }

            shadowPercentage = shadowPercentage/121.0;

            vec3 color = fragmentColor.xyz * shadowPercentage * 0.5 + fragmentColor.xyz * 0.5;

            fragmentColor = vec4(color, 1.0);

            //vec3 color = shadow + shadowPercentage*diffuse + shadowPercentage*phong;
            
            // if (texture_enable > 0) {
            //     fragmentColor = texture(texture_diffuse1, TexCoord) * vec4(color, 1.0);
            // } else {
            //     fragmentColor = vec4(color, 1.0);
            // }

            //fragmentColor = fragmentColor * vec4(shadowPercentage*0.7 + 0.3, shadowPercentage*0.7 + 0.3, shadowPercentage*0.7 + 0.3, 1.0);
        }
    }
}