#version 330 core
precision highp float;
out highp vec4 FragColor;

in vec4 vertexPosition;

uniform float roughness;
uniform vec3 modelColor;
uniform vec3 cameraPosition;
uniform vec2 screenResolution;

uniform highp mat4 world2screenMatrix;
uniform highp mat4 camera2screenMatrix;
uniform highp mat4 world2cameraMatrix;

uniform sampler2D normalMapSampler2D;
uniform sampler2D depthMapSampler2D;
uniform sampler2D directShadingSampler2D;

uniform bool SSR;

const float PI = 3.14159265359;

const uint SAMPLE_COUNT = 8u;

in vec4 glPosition;

float LinearizeDepth(float depth) 
{
    float near = 0.1;
    float far = 10000.0;
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);    
}

// BrdfDividePDF = Brdf*cosTheta/PDF
vec3 BrdfDividePDF(vec3 V, vec3 N, vec3 L, float roughness, vec3 fresnel)
{
    V = normalize(V);
    L = normalize(L);
    N = normalize(N);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float HdotV = max(dot(H, V), 0.0);
    float VdotN = max(dot(N, V), 0.0);

    vec3 F =  fresnel + (vec3(1.0) - fresnel)*pow((1.0 - HdotV), 5.0);

    float k = roughness*roughness/2.0;
    float G1 = VdotN/(VdotN*(1.0 - k) + k);
    float G2 = NdotL/(NdotL*(1.0 - k) + k);
    float G = G1*G2;

    vec3 BrdfDividePDF = F*G*HdotV/(VdotN*NdotH);
    return BrdfDividePDF;
}

highp float getDepth(highp float x, bool if_x, highp float y, bool if_y, highp vec3 origin, highp vec3 reflect)
{
    highp float t = 0.0;
    reflect = normalize(reflect);

    highp mat4 A = world2screenMatrix;

    highp float w;

    highp float w_a = A[0][3]*reflect[0] + A[1][3]*reflect[1] + A[2][3]*reflect[2];
    highp float w_b = A[0][3]*origin[0] + A[1][3]*origin[1] + A[2][3]*origin[2] + A[3][3];

    if (if_x) {
        t = (w_b*x - (A[0][0]*origin[0] + A[1][0]*origin[1] + A[2][0]*origin[2] + A[3][0]))/(A[0][0]*reflect[0] + A[1][0]*reflect[1] + A[2][0]*reflect[2] - w_a*x);
        w = w_a*t + w_b;
    }

    if (if_y) {
        t = (w_b*y - (A[0][1]*origin[0] + A[1][1]*origin[1] + A[2][1]*origin[2] + A[3][1]))/(A[0][1]*reflect[0] + A[1][1]*reflect[1] + A[2][1]*reflect[2] - w_a*y);
        w = w_a*t + w_b;
    }

    highp float depth = t*(A[0][2]*reflect[0] + A[1][2]*reflect[1] + A[2][2]*reflect[2]) + (A[0][2]*origin[0] + A[1][2]*origin[1] + A[2][2]*origin[2] + A[3][2]);

    depth = depth/w;

    float near = 0.1;
    float far = 10000.0;
    return (2.0 * near * far) / (far + near - depth * (far - near)); 
}

vec4 getIndirctLight(vec3 origin, vec3 reflect, vec2 screenCoord, vec2 direction) 
{
    float x =  2.0*screenCoord.x/screenResolution.x - 1.0;
    float depthOrigin = getDepth(x, true, 0, false, origin, reflect);
    vec2 st = screenCoord.xy/screenResolution;
    float localDepthOrigin = glPosition.z/glPosition.w;;

    highp float ratio = (direction.y - screenCoord.y)/(direction.x - screenCoord.x);
    if (abs(ratio) <= 1) {
        highp float x_step = (direction.x - screenCoord.x)/abs(direction.x - screenCoord.x);
        x_step = x_step;
        float step_level = 1.0;
        for (float x = screenCoord.x + x_step; (x > 0.0 && x < screenResolution.x); x = x + step_level*x_step) {
            highp float projectionCoord = 2.0*x/screenResolution.x - 1.0;
            highp float depth = getDepth(projectionCoord, true, 0, false, origin, reflect);
            highp vec2 st = vec2(x, screenCoord.y + ratio*(x - screenCoord.x))/screenResolution;
            highp float localDepth = LinearizeDepth(textureLod(depthMapSampler2D, st, 0).r);

            highp float delta = 0.5;

            if (depth > localDepth) {
                if (depth < 0.1 || depth > 10000.0 || depth > (localDepth + delta)) {
                    return vec4(0.0);
                }

                vec4 intersection = vec4(x, (screenCoord.y + ratio*(x - screenCoord.x)), 0.0 ,1.0);
                return intersection;

            }
        }
        vec4 outScreen = vec4(0.0);
        return outScreen;
    } else {
        ratio = 1.0/ratio;
        highp float y_step = (direction.y - screenCoord.y)/abs(direction.y - screenCoord.y);
        y_step = y_step;
        float step_level = 1.0;
        for (float y = screenCoord.y + y_step; (y > 0.0 && y < screenResolution.y); y = y + step_level*y_step) {
            highp float projectionCoord = 2.0*y/screenResolution.y - 1.0;
            highp float depth = getDepth(0, false, projectionCoord, true, origin, reflect);
            highp vec2 st = vec2(screenCoord.x + ratio*(y - screenCoord.y), y)/screenResolution;
            highp float localDepth = LinearizeDepth(textureLod(depthMapSampler2D, st, 0).r);

            highp float delta = 0.5;

            if (depth > localDepth) {

                if (depth < 0.1 || depth > 10000.0 || depth > (localDepth + delta)) {
                    return vec4(0.0);
                }

                vec4 intersection = vec4((screenCoord.x + ratio*(y - screenCoord.y)), y, 0.0, 1.0);
                return intersection;

            }
        }
        vec4 outScreen = vec4(0.0);
        return outScreen;
    }
}

vec2 getTextureCoordFromNearestDepthIn3D(vec3 origin, vec3 reflect, float depth)
{
    highp float linearDepth = -LinearizeDepth(depth);
    highp vec4 direction = world2cameraMatrix * vec4(origin + reflect, 1.0);
    origin = (world2cameraMatrix * vec4(origin, 1.0)).xyz;
    reflect = direction.xyz - origin;
    highp float t = (linearDepth - origin.z)/reflect.z;
    highp float x = origin.x + t * reflect.x;
    highp float y = origin.y + t * reflect.y;
    highp vec4 insertPoint = vec4(x, y, linearDepth, 1.0);
    highp vec4 result = camera2screenMatrix * insertPoint;
    highp vec2 textureCoord = (vec2(result.xy/result.w) + vec2(1.0)) * 0.5;
    return textureCoord;
}

vec4 getIndirctLightHiz(vec3 origin, vec3 reflect, vec2 screenCoord, vec2 direction) 
{
    highp float ratio = (direction.y - screenCoord.y)/(direction.x - screenCoord.x);
    highp float yStep = 0.0f, xStep = 0.0f;
    if (abs(ratio) <= 1.0f) {
        xStep = (direction.x - screenCoord.x)/abs(direction.x - screenCoord.x);
    } else {
        ratio = 1.0f/ratio;
        yStep = (direction.y - screenCoord.y)/abs(direction.y - screenCoord.y);
    }

    int currentMipmapLevel = 0;
    ivec2 currentPixelCoord = ivec2(gl_FragCoord.xy);

    while(currentPixelCoord.x > 0 && currentPixelCoord.x < textureSize(depthMapSampler2D, currentMipmapLevel).x && currentPixelCoord.y > 0 && currentPixelCoord.y < textureSize(depthMapSampler2D, currentMipmapLevel).y) 
    {
	    highp float nearestDepth = texelFetch(depthMapSampler2D, currentPixelCoord, currentMipmapLevel).r;

        vec2 rayPixelCoord = getTextureCoordFromNearestDepthIn3D(origin, reflect, nearestDepth) * textureSize(depthMapSampler2D, currentMipmapLevel);

        if (abs(float(currentPixelCoord.x) + 0.5 - rayPixelCoord.x) < 2.0 && abs(float(currentPixelCoord.y) + 0.5 - rayPixelCoord.y) < 2.0 && currentPixelCoord != ivec2(gl_FragCoord.xy)) {
            if (0 == currentMipmapLevel)
            {
                    if (nearestDepth < 1.0f) {
                        vec4 intersection = vec4(vec2(rayPixelCoord), 0.0, 1.0);
                        return intersection;
                    } else {
                        vec4 outScreen = vec4(0.8);
                        return outScreen;                     
                    }
            } else {
                if (currentMipmapLevel > 0) {
                    currentMipmapLevel--;
                
                    if (xStep > 0.0) {
                        currentPixelCoord.x = 2 * currentPixelCoord.x;
                        highp float x = float(currentPixelCoord.x)/textureSize(depthMapSampler2D, currentMipmapLevel).x;
                        highp float y = screenCoord.y + ratio * (x - screenCoord.x);
                        currentPixelCoord.y = int(y * textureSize(depthMapSampler2D, currentMipmapLevel).y);
                    }

                    if (xStep < 0.0) {
                        currentPixelCoord.x = 2 * currentPixelCoord.x + 1;
                        highp float x = float(currentPixelCoord.x)/textureSize(depthMapSampler2D, currentMipmapLevel).x;
                        highp float y = screenCoord.y + ratio * (x - screenCoord.x);
                        currentPixelCoord.y = int(y * textureSize(depthMapSampler2D, currentMipmapLevel).y);
                    }

                    if (yStep > 0.0) {
                        currentPixelCoord.y = 2 * currentPixelCoord.y;
                        highp float y = float(currentPixelCoord.y)/textureSize(depthMapSampler2D, currentMipmapLevel).y;
                        highp float x = screenCoord.x + ratio * (y - screenCoord.y);
                        currentPixelCoord.x = int(x * textureSize(depthMapSampler2D, currentMipmapLevel).x);
                    }

                    if (yStep < 0.0) {
                        currentPixelCoord.y =  2 * currentPixelCoord.y + 1;
                        highp float y = float(currentPixelCoord.y)/textureSize(depthMapSampler2D, currentMipmapLevel).y;
                        highp float x = screenCoord.x + ratio * (y - screenCoord.y);
                        currentPixelCoord.x = int(x * textureSize(depthMapSampler2D, currentMipmapLevel).x);
                    }
                }
            }
        } else {
            ivec2 nextPixelCoord;
            bool equal;

            if (yStep != 0.0)
            {
                int step = (yStep > 0 ? 1 : -1);
                nextPixelCoord.y = currentPixelCoord.y + step;
                highp float y = float(nextPixelCoord.y)/textureSize(depthMapSampler2D, currentMipmapLevel).y;
                highp float x = screenCoord.x + ratio * (y - screenCoord.y);
                nextPixelCoord.x = int(x * textureSize(depthMapSampler2D, currentMipmapLevel).x);
                if (nextPixelCoord.x != currentPixelCoord.x) {
                    nextPixelCoord.y = currentPixelCoord.y;
                }
                equal = (currentPixelCoord.y/2 != nextPixelCoord.y/2) ? false : true;
            }
            
            if (xStep != 0.0)
            {
                int step = (xStep > 0 ? 1 : -1);
                nextPixelCoord.x = currentPixelCoord.x + step;
                highp float x = float(nextPixelCoord.x)/textureSize(depthMapSampler2D, currentMipmapLevel).x;
                highp float y = screenCoord.y + ratio * (x - screenCoord.x);
                nextPixelCoord.y = int(y * textureSize(depthMapSampler2D, currentMipmapLevel).y);
                if (nextPixelCoord.y != currentPixelCoord.y) {
                    nextPixelCoord.x = currentPixelCoord.x;
                }
                equal = (currentPixelCoord.x/2 != nextPixelCoord.x/2) ? false : true;
            }

            if ((!equal) && currentMipmapLevel <= 9) {
                currentMipmapLevel++;
                currentPixelCoord = nextPixelCoord/2;
            } else {
                currentPixelCoord = nextPixelCoord;
            }
        }
    }

    vec4 outScreen = vec4(0.0);
    return outScreen; 
}

float haltonValue(int index, int base) {
    float result = 0.0;
    float fraction = 1.0 / base;
    int currentIndex = index;
    while (currentIndex > 0) {
        result += (currentIndex % base) * fraction;
        currentIndex /= base;
        fraction /= base;
    }
    return result;
}

void main()
{    
    vec3 V = normalize(cameraPosition - vertexPosition.xyz);
    vec2 st = gl_FragCoord.xy/screenResolution;
    vec3 N = textureLod(normalMapSampler2D, st, 0).xyz;

    vec3 directLightShading = texture(directShadingSampler2D, st).xyz;

    if (!SSR) {
        FragColor = vec4(directLightShading, 1.0);
        return;
    }

    vec3 globalLight = vec3(0.0);
    float sampleObject = 0;
    float sampleEnvironment = 0;

    for(uint i = 1u; i <= SAMPLE_COUNT; ++i)
    {
        int randomseed = int(gl_FragCoord.x + gl_FragCoord.y * textureSize(depthMapSampler2D, 0).x);
        vec2 Xi;
        Xi.x = haltonValue(randomseed*int(i), 11);
        Xi.y = haltonValue(randomseed*int(i), 7);
        float bias = 0.7;
        Xi.y = mix(Xi.y, 0.0, bias);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        if(NdotL > 0.0)
        {
            vec3 reflectLightSecondPoint = vertexPosition.xyz + L;
            vec4 noNormalizationCoord = world2screenMatrix * vec4(reflectLightSecondPoint, 1.0);
            noNormalizationCoord = noNormalizationCoord/noNormalizationCoord.w;
            vec2 screenCoord = vec2(noNormalizationCoord.xy);
            screenCoord = (screenCoord + vec2(1.0)) * 0.5;
            vec2 pixelCoord = screenCoord * screenResolution;

            // vec4 result = getIndirctLight(vertexPosition.xyz, L, gl_FragCoord.xy, pixelCoord);

            vec4 vertexCoord = world2screenMatrix * vertexPosition;
            vertexCoord = vertexCoord/vertexCoord.w;
            
            vec4 result = getIndirctLightHiz(vertexPosition.xyz, L, (vertexCoord.xy + vec2(1.0)) * 0.5, screenCoord);

            if (result.w == 1.0) {
                vec3 indirectLight = texture(directShadingSampler2D, result.xy/screenResolution).xyz;

                indirectLight = indirectLight*BrdfDividePDF(V, N, L, roughness, modelColor);

                sampleObject = sampleObject + 1.0;
                globalLight = globalLight + indirectLight;
            } else {
                sampleEnvironment = sampleEnvironment + 1.0;
                globalLight = globalLight + directLightShading;
            }
        }
    }

    globalLight = globalLight/(sampleObject + sampleEnvironment);

    FragColor = vec4(globalLight, 1.0);
}