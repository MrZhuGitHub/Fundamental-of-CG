#version 330 core
precision highp float;
out highp vec4 FragColor;

in vec4 vertexPosition;

uniform float roughness;
uniform vec3 modelColor;
uniform vec3 cameraPosition;
uniform vec2 screenResolution;

uniform highp mat4 world2screenMatrix;

uniform sampler2D normalMapSampler2D;
uniform sampler2D depthMapSampler2D;
uniform sampler2D directShadingSampler2D;

uniform bool SSR;

const float PI = 3.14159265359;

const uint SAMPLE_COUNT = 10u;

in vec4 glPosition;

float LinearizeDepth(float depth) 
{
    float near = 10.0;
    float far = 1000.0;
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
vec3 BrdfDividePDF(vec3 V, vec3 L, vec3 N, float roughness, vec3 fresnel)
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

    float near = 10.0;
    float far = 1000.0;
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
            highp float localDepth = LinearizeDepth(texture(depthMapSampler2D, st).r);

            // highp float previous = x - x_step;
            // highp float previousProjectionCoord = 2.0*previous/screenResolution.x - 1.0;
            // highp float previousDepth = getDepth(previousProjectionCoord, true, 0, false, origin, reflect);

            highp float delta = 1.0;

            if (depth > localDepth) {
                if (depth < 10.0 || depth > 1000.0 || depth > (localDepth + delta)) {
                    return vec4(0.0);
                }

                vec4 intersection = vec4(x, (screenCoord.y + ratio*(x - screenCoord.x)), 0.0 ,1.0);
                return intersection;

            }

            // if ((depth) > (localDepth + 0.025)) {
            //     if (step_level > 1.0) {
            //         x = x - step_level*x_step;
            //         step_level = step_level/2.0;
            //     } else {
            //         float previous = x - step_level*x_step;
            //         float previousProjectionCoord = 2.0*previous/screenResolution.x - 1.0;
            //         float previousDepth = getDepth(previousProjectionCoord, true, 0, false, origin, reflect);
            //         if ((previousDepth) < localDepth && depth > -1.0 && depth < 1.0) {
            //             //vec4 intersection = vec4(1.0);
            //             //vec4 intersection = vec4((x - screenCoord.x)/10000.0, ratio*(x - screenCoord.x)/10000.0, abs(depth - depthOrigin), localDepthOrigin);
            //             vec4 intersection = vec4(x, (screenCoord.y + ratio*(x - screenCoord.x)), delta ,1.0);
            //             return intersection;
            //         } else {
            //             vec4 block = vec4(0.4);
            //             return block;
            //         }
            //     }
            // } else {
            //     step_level = step_level * 2.0;
            // }
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
            highp float localDepth = LinearizeDepth(texture(depthMapSampler2D, st).r);

            // highp float previous = y - y_step;
            // highp float previousProjectionCoord = 2.0*previous/screenResolution.x - 1.0;
            // highp float previousDepth = getDepth(0, false, previousProjectionCoord, true, origin, reflect);

            highp float delta = 1.0;

            if (depth > localDepth) {

                if (depth < 10.0 || depth > 1000.0 || depth > (localDepth + delta)) {
                    return vec4(0.0);
                }

                vec4 intersection = vec4((screenCoord.x + ratio*(y - screenCoord.y)), y, 0.0, 1.0);
                return intersection;

            }

            // if ((depth) > (localDepth + 0.025)) {
            //     if (step_level > 1.0) {
            //         y = y - step_level*y_step;
            //         step_level = step_level/2.0;
            //     } else {
            //         float previous = y - step_level*y_step;
            //         float previousProjectionCoord = 2.0*previous/screenResolution.x - 1.0;
            //         float previousDepth = getDepth(0, false, previousProjectionCoord, true, origin, reflect);
            //         if ((previousDepth) < localDepth) {
            //             //vec4 intersection = vec4(1.0);
            //             //vec4 intersection = vec4(ratio*(y - screenCoord.y)/10000.0f, (y - screenCoord.y)/10000.0f,  abs(depth - depthOrigin), localDepthOrigin);
            //             vec4 intersection = vec4((screenCoord.x + ratio*(y - screenCoord.y)), y, delta, 1.0);
            //             return intersection;
            //         }
            //         } else {
            //             vec4 block = vec4(0.4);
            //             return block;
            //         }

            //     }
            // } else {
            //     step_level = step_level * 2.0;
            // }
        }
        vec4 outScreen = vec4(0.0);
        return outScreen;
    }
}

void main()
{    
    vec3 V = normalize(cameraPosition - vertexPosition.xyz);
    vec2 st = gl_FragCoord.xy/screenResolution;
    vec3 N = texture(normalMapSampler2D, st).xyz;

    float localDepth = LinearizeDepth(texture(depthMapSampler2D, st).r);

    vec3 directLightShading = texture(directShadingSampler2D, st).xyz;

    if (!SSR) {
        FragColor = vec4(directLightShading, 1.0);
        return;
    }

    vec3 globalLight = vec3(0.0);
    float sampleObject = 0;
    float sampleEnvironment = 0;

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
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
            screenCoord = (screenCoord + vec2(1.0))*0.5*screenResolution;

            vec4 result = getIndirctLight(vertexPosition.xyz, L, gl_FragCoord.xy, screenCoord);

            // FragColor = result;
            // return;

            if (result.w == 1.0) {
                vec3 indirectLight = texture(directShadingSampler2D, result.xy/screenResolution).xyz;

                // indirectLight = indirectLight*BrdfDividePDF(V, N, L, roughness, modelColor);

                sampleObject = sampleObject + 1.0;
                globalLight = globalLight + indirectLight;
            } else {
                sampleEnvironment = sampleEnvironment + 1.0;
                globalLight = globalLight + directLightShading;
            }
        }
    }

    globalLight = globalLight/(sampleObject + sampleEnvironment);
    FragColor = vec4(globalLight, localDepth);

    // FragColor = vec4(0.6, 0.6, 0.6, 1.0);
}